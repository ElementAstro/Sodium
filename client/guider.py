import copy
import json
import math
import selectors
import socket
import threading
import time
from dataclasses import dataclass, field


@dataclass
class SettleProgress:
    Done: bool = False
    Distance: float = 0.0
    SettlePx: float = 0.0
    Time: float = 0.0
    SettleTime: float = 0.0
    Status: int = 0
    Error: str = ''


@dataclass
class GuideStats:
    rms_tot: float = 0.0
    rms_ra: float = 0.0
    rms_dec: float = 0.0
    peak_ra: float = 0.0
    peak_dec: float = 0.0


class GuiderException(Exception):
    pass


class _Accum:
    def __init__(self):
        self.Reset()

    def Reset(self):
        self.n = 0
        self.a = self.q = self.peak = 0

    def Add(self, x):
        ax = abs(x)
        self.peak = max(self.peak, ax)
        self.n += 1
        d = x - self.a
        self.a += d / self.n
        self.q += d * (x - self.a)

    def Mean(self):
        return self.a

    def Stdev(self):
        return math.sqrt(self.q / self.n) if self.n >= 1 else 0.0

    def Peak(self):
        return self.peak


class _Conn:
    def __init__(self):
        self.lines = []
        self.buf = b''
        self.sock = None
        self.sel = None
        self.terminate = False

    def __del__(self):
        self.Disconnect()

    def Connect(self, hostname, port):
        self.sock = socket.socket()
        try:
            self.sock.connect((hostname, port))
            self.sock.setblocking(False)
            self.sel = selectors.DefaultSelector()
            self.sel.register(self.sock, selectors.EVENT_READ)
        except Exception:
            self.sel = self.sock = None
            raise

    def Disconnect(self):
        if self.sel:
            self.sel.unregister(self.sock)
            self.sel = None
        if self.sock:
            self.sock.close()
            self.sock = None

    def IsConnected(self):
        return self.sock is not None

    def ReadLine(self):
        while not self.lines:
            while not self.terminate:
                events = self.sel.select(0.5)
                if events:
                    break
            s = self.sock.recv(4096)
            i0 = 0
            for i in range(len(s)):
                if s[i] in (b'\r'[0], b'\n'[0]):
                    self.buf += s[i0:i]
                    if self.buf:
                        self.lines.append(self.buf)
                        self.buf = b''
                    i0 = i + 1
            self.buf += s[i0:]
        return self.lines.pop(0)

    def WriteLine(self, s):
        b = s.encode()
        totsent = 0
        while totsent < len(b):
            sent = self.sock.send(b[totsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totsent += sent

    def Terminate(self):
        self.terminate = True


class Guider:
    DEFAULT_STOPCAPTURE_TIMEOUT = 10

    def __init__(self, hostname="localhost", instance=1):
        self.hostname = hostname
        self.instance = instance
        self.conn = None
        self.terminate = False
        self.worker = None
        self.lock = threading.Lock()
        self.cond = threading.Condition()
        self.response = None
        self.AppState = ''
        self.AvgDist = 0
        self.Version = ''
        self.PHDSubver = ''
        self.accum_active = False
        self.settle_px = 0
        self.accum_ra = _Accum()
        self.accum_dec = _Accum()
        self.Stats = GuideStats()
        self.Settle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.Disconnect()

    @staticmethod
    def _is_guiding(st):
        return st in {"Guiding", "LostLock"}

    @staticmethod
    def _accum_get_stats(ra, dec):
        return GuideStats(
            rms_ra=ra.Stdev(),
            rms_dec=dec.Stdev(),
            peak_ra=ra.Peak(),
            peak_dec=dec.Peak()
        )

    def _handle_event(self, ev):
        e = ev["Event"]
        with self.lock:
            if e == "AppState":
                self.AppState = ev["State"]
                if self._is_guiding(self.AppState):
                    self.AvgDist = 0
            elif e == "Version":
                self.Version = ev["SodiumVersion"]
                self.PHDSubver = ev["SodiumSubver"]
            elif e == "StartGuiding":
                self.accum_active = True
                self.accum_ra.Reset()
                self.accum_dec.Reset()
                self.Stats = self._accum_get_stats(
                    self.accum_ra, self.accum_dec)
            elif e == "GuideStep" and self.accum_active:
                self.accum_ra.Add(ev["RADistanceRaw"])
                self.accum_dec.Add(ev["DECDistanceRaw"])
                self.Stats = self._accum_get_stats(
                    self.accum_ra, self.accum_dec)
                self.AppState = "Guiding"
                self.AvgDist = ev["AvgDist"]
            elif e == "SettleBegin":
                self.accum_active = False
            elif e == "Settling":
                self.Settle = SettleProgress(
                    Done=False,
                    Distance=ev["Distance"],
                    SettlePx=self.settle_px,
                    Time=ev["Time"],
                    SettleTime=ev["SettleTime"]
                )
            elif e == "SettleDone":
                self.accum_active = True
                self.accum_ra.Reset()
                self.accum_dec.Reset()
                self.Stats = self._accum_get_stats(
                    self.accum_ra, self.accum_dec)
                self.Settle = SettleProgress(
                    Done=True,
                    Status=ev["Status"],
                    Error=ev.get("Error")
                )
            elif e == "Paused":
                self.AppState = "Paused"
            elif e == "StartCalibration":
                self.AppState = "Calibrating"
            elif e == "LoopingExposures":
                self.AppState = "Looping"
            elif e in {"LoopingExposuresStopped", "GuidingStopped"}:
                self.AppState = "Stopped"
            elif e == "StarLost":
                self.AppState = "LostLock"
                self.AvgDist = ev["AvgDist"]

    def _worker(self):
        while not self.terminate:
            line = self.conn.ReadLine()
            if not line and not self.terminate:
                break
            try:
                j = json.loads(line)
            except json.JSONDecodeError:
                continue
            if "jsonrpc" in j:
                with self.cond:
                    self.response = j
                    self.cond.notify()
            else:
                self._handle_event(j)

    def Connect(self):
        self.Disconnect()
        try:
            self.conn = _Conn()
            self.conn.Connect(self.hostname, 4400 + self.instance - 1)
            self.terminate = False
            self.worker = threading.Thread(target=self._worker)
            self.worker.start()
        except Exception:
            self.Disconnect()
            raise

    def Disconnect(self):
        if self.worker and self.worker.is_alive():
            self.terminate = True
            self.conn.Terminate()
            self.worker.join()
        self.worker = None
        if self.conn:
            self.conn.Disconnect()
            self.conn = None

    @staticmethod
    def _make_jsonrpc(method, params):
        req = {"method": method, "id": 1}
        if params:
            req["params"] = params if isinstance(
                params, (list, dict)) else [params]
        return json.dumps(req, separators=(',', ':'))

    @staticmethod
    def _failed(res):
        return "error" in res

    def Call(self, method, params=None):
        s = self._make_jsonrpc(method, params)
        self.conn.WriteLine(s + "\r\n")
        with self.cond:
            while not self.response:
                self.cond.wait()
            response = self.response
            self.response = None
        if self._failed(response):
            raise GuiderException(response["error"]["message"])
        return response

    def _CheckConnected(self):
        if not self.conn.IsConnected():
            raise GuiderException("PHD2 Server disconnected")

    def Guide(self, settlePixels, settleTime, settleTimeout):
        self._CheckConnected()
        with self.lock:
            if self.Settle and not self.Settle.Done:
                raise GuiderException("cannot guide while settling")
            self.Settle = SettleProgress(
                Done=False,
                Distance=0,
                SettlePx=settlePixels,
                Time=0,
                SettleTime=settleTime
            )
        try:
            self.Call("guide", [
                      {"pixels": settlePixels, "time": settleTime, "timeout": settleTimeout}, False])
            self.settle_px = settlePixels
        except Exception:
            with self.lock:
                self.Settle = None
            raise

    def Dither(self, ditherPixels, settlePixels, settleTime, settleTimeout):
        self._CheckConnected()
        with self.lock:
            if self.Settle and not self.Settle.Done:
                raise GuiderException("cannot dither while settling")
            self.Settle = SettleProgress(
                Done=False,
                Distance=ditherPixels,
                SettlePx=settlePixels,
                Time=0,
                SettleTime=settleTime
            )
        try:
            self.Call("dither", [
                ditherPixels, False,
                {"pixels": settlePixels, "time": settleTime, "timeout": settleTimeout}
            ])
            self.settle_px = settlePixels
        except Exception:
            with self.lock:
                self.Settle = None
            raise

    def IsSettling(self):
        self._CheckConnected()
        with self.lock:
            if self.Settle:
                return True
        res = self.Call("get_settling")
        val = res["result"]
        if val:
            with self.lock:
                if not self.Settle:
                    self.Settle = SettleProgress(Done=False, Distance=-1.0)
        return val

    def CheckSettling(self):
        self._CheckConnected()
        ret = SettleProgress()
        with self.lock:
            if not self.Settle:
                raise GuiderException("not settling")
            if self.Settle.Done:
                ret = copy.deepcopy(self.Settle)
                self.Settle = None
            else:
                ret = copy.deepcopy(self.Settle)
                ret.SettlePx = self.settle_px
        return ret

    def GetStats(self):
        self._CheckConnected()
        with self.lock:
            stats = copy.copy(self.Stats)
        stats.rms_tot = math.hypot(stats.rms_ra, stats.rms_dec)
        return stats

    def StopCapture(self, timeoutSeconds=10):
        self.Call("stop_capture")
        for _ in range(timeoutSeconds):
            with self.lock:
                if self.AppState == "Stopped":
                    return
            time.sleep(1)
            self._CheckConnected()
        res = self.Call("get_app_state")
        st = res["result"]
        with self.lock:
            self.AppState = st
        if st != "Stopped":
            raise GuiderException(f"guider did not stop capture after {
                                  timeoutSeconds} seconds!")

    def Loop(self, timeoutSeconds=10):
        self._CheckConnected()
        with self.lock:
            if self.AppState == "Looping":
                return
        res = self.Call("get_exposure")
        exp_ms = res["result"]
        self.Call("loop")
        time.sleep(exp_ms / 1000)
        for _ in range(timeoutSeconds):
            with self.lock:
                if self.AppState == "Looping":
                    return
            time.sleep(1)
            self._CheckConnected()
        raise GuiderException("timed-out waiting for guiding to start looping")

    def PixelScale(self):
        res = self.Call("get_pixel_scale")
        return res["result"]

    def GetEquipmentProfiles(self):
        res = self.Call("get_profiles")
        return [p["name"] for p in res["result"]]

    def ConnectEquipment(self, profileName):
        res = self.Call("get_profile")
        prof = res["result"]
        if prof["name"] != profileName:
            res = self.Call("get_profiles")
            profiles = res["result"]
            profid = next((p.get("id", -1)
                          for p in profiles if p["name"] == profileName), -1)
            if profid == -1:
                raise GuiderException(
                    f"invalid phd2 profile name: {profileName}")
            self.StopCapture(self.DEFAULT_STOPCAPTURE_TIMEOUT)
            self.Call("set_connected", False)
            self.Call("set_profile", profid)
        self.Call("set_connected", True)

    def DisconnectEquipment(self):
        self.StopCapture(self.DEFAULT_STOPCAPTURE_TIMEOUT)
        self.Call("set_connected", False)

    def GetStatus(self):
        self._CheckConnected()
        with self.lock:
            return self.AppState, self.AvgDist

    def IsGuiding(self):
        st, _ = self.GetStatus()
        return self._is_guiding(st)

    def Pause(self):
        self.Call("set_paused", True)

    def Unpause(self):
        self.Call("set_paused", False)

    def SaveImage(self):
        res = self.Call("save_image")
        return res["result"]["filename"]
