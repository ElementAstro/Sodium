/*
 * event_server.cpp
 *
 * Copyright (C) 2023 Max Qian <lightapt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*************************************************

Copyright: 2023 Max Qian. All rights reserved

Author: Max Qian

E-mail: astro_air@126.com

Date: 2023-3-5

Description: Socket server

**************************************************/

#include "darks_dialog.hpp"
#include "myframe.hpp"
#include "sodium.hpp"

#include <spdlog/spdlog.h>

#include <string.h>
#include <wx/sckstrm.h>
#include <wx/sstream.h>
#include <cstddef>
#include <sstream>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

EventServer EvtServer;

BEGIN_EVENT_TABLE(EventServer, wxEvtHandler)
EVT_SOCKET(EVENT_SERVER_ID, EventServer::OnEventServerEvent)
EVT_SOCKET(EVENT_SERVER_CLIENT_ID, EventServer::OnEventServerClientEvent)
END_EVENT_TABLE()

enum {
    MSG_PROTOCOL_VERSION = 1,
};

static const wxString literal_null("null");
static const wxString literal_true("true");
static const wxString literal_false("false");

static wxString state_name(EXPOSED_STATE st) {
    switch (st) {
        case EXPOSED_STATE_NONE:
            return "Stopped";
        case EXPOSED_STATE_SELECTED:
            return "Selected";
        case EXPOSED_STATE_CALIBRATING:
            return "Calibrating";
        case EXPOSED_STATE_GUIDING_LOCKED:
            return "Guiding";
        case EXPOSED_STATE_GUIDING_LOST:
            return "LostLock";
        case EXPOSED_STATE_PAUSED:
            return "Paused";
        case EXPOSED_STATE_LOOPING:
            return "Looping";
        default:
            return "Unknown";
    }
}

static wxString json_escape(const wxString &s) {
    wxString t(s);
    static const wxString BACKSLASH("\\");
    static const wxString BACKSLASHBACKSLASH("\\\\");
    static const wxString DQUOT("\"");
    static const wxString BACKSLASHDQUOT("\\\"");
    static const wxString CR("\r");
    static const wxString BACKSLASHCR("\\r");
    static const wxString LF("\n");
    static const wxString BACKSLASHLF("\\n");
    t.Replace(BACKSLASH, BACKSLASHBACKSLASH);
    t.Replace(DQUOT, BACKSLASHDQUOT);
    t.Replace(CR, BACKSLASHCR);
    t.Replace(LF, BACKSLASHLF);
    return t;
}

template <char LDELIM, char RDELIM>
struct JSeq {
    wxString m_s;
    bool m_first;
    bool m_closed;
    JSeq() : m_first(true), m_closed(false) { m_s << LDELIM; }
    void close() {
        m_s << RDELIM;
        m_closed = true;
    }
    wxString str() {
        if (!m_closed)
            close();
        return m_s;
    }
};

typedef JSeq<'[', ']'> JAry;
typedef JSeq<'{', '}'> JObj;

static JAry &operator<<(JAry &a, const wxString &str) {
    if (a.m_first)
        a.m_first = false;
    else
        a.m_s << ',';
    a.m_s << str;
    return a;
}

static JAry &operator<<(JAry &a, double d) {
    return a << wxString::Format("%.2f", d);
}

static JAry &operator<<(JAry &a, int i) {
    return a << wxString::Format("%d", i);
}

static wxString json_format(const json_value *j) {
    if (!j)
        return literal_null;

    switch (j->type) {
        default:
        case JSON_NULL:
            return literal_null;
        case JSON_OBJECT: {
            wxString ret("{");
            bool first = true;
            json_for_each(jj, j) {
                if (first)
                    first = false;
                else
                    ret << ",";
                ret << '"' << jj->name << "\":" << json_format(jj);
            }
            ret << "}";
            return ret;
        }
        case JSON_ARRAY: {
            wxString ret("[");
            bool first = true;
            json_for_each(jj, j) {
                if (first)
                    first = false;
                else
                    ret << ",";
                ret << json_format(jj);
            }
            ret << "]";
            return ret;
        }
        case JSON_STRING:
            return '"' + json_escape(j->string_value) + '"';
        case JSON_INT:
            return wxString::Format("%d", j->int_value);
        case JSON_FLOAT:
            return wxString::Format("%g", (double)j->float_value);
        case JSON_BOOL:
            return j->int_value ? literal_true : literal_false;
    }
}

struct NULL_TYPE {
} NULL_VALUE;

// name-value pair
struct NV {
    wxString n;
    wxString v;
    NV(const wxString &n_, const wxString &v_)
        : n(n_), v('"' + json_escape(v_) + '"') {}
    NV(const wxString &n_, const char *v_)
        : n(n_), v('"' + json_escape(v_) + '"') {}
    NV(const wxString &n_, const wchar_t *v_)
        : n(n_), v('"' + json_escape(v_) + '"') {}
    NV(const wxString &n_, int v_) : n(n_), v(wxString::Format("%d", v_)) {}
    NV(const wxString &n_, unsigned int v_)
        : n(n_), v(wxString::Format("%u", v_)) {}
    NV(const wxString &n_, double v_) : n(n_), v(wxString::Format("%g", v_)) {}
    NV(const wxString &n_, double v_, int prec)
        : n(n_), v(wxString::Format("%.*f", prec, v_)) {}
    NV(const wxString &n_, bool v_)
        : n(n_), v(v_ ? literal_true : literal_false) {}
    template <typename T>
    NV(const wxString &n_, const std::vector<T> &vec);
    NV(const wxString &n_, JAry &ary) : n(n_), v(ary.str()) {}
    NV(const wxString &n_, JObj &obj) : n(n_), v(obj.str()) {}
    NV(const wxString &n_, const json_value *v_) : n(n_), v(json_format(v_)) {}
    NV(const wxString &n_, const SodiumPoint &p) : n(n_) {
        JAry ary;
        ary << p.X << p.Y;
        v = ary.str();
    }
    NV(const wxString &n_, const wxPoint &p) : n(n_) {
        JAry ary;
        ary << p.x << p.y;
        v = ary.str();
    }
    NV(const wxString &n_, const wxSize &s) : n(n_) {
        JAry ary;
        ary << s.x << s.y;
        v = ary.str();
    }
    NV(const wxString &n_, const NULL_TYPE &nul) : n(n_), v(literal_null) {}
};

template <typename T>
NV::NV(const wxString &n_, const std::vector<T> &vec) : n(n_) {
    std::ostringstream os;
    os << '[';
    for (unsigned int i = 0; i < vec.size(); i++) {
        if (i != 0)
            os << ',';
        os << vec[i];
    }
    os << ']';
    v = os.str();
}

static JObj &operator<<(JObj &j, const NV &nv) {
    if (j.m_first)
        j.m_first = false;
    else
        j.m_s << ',';
    j.m_s << '"' << nv.n << "\":" << nv.v;
    return j;
}

static NV NVMount(const Mount *mount) { return NV("Mount", mount->Name()); }

static JObj &operator<<(JObj &j, const SodiumPoint &pt) {
    return j << NV("X", pt.X, 3) << NV("Y", pt.Y, 3);
}

static JAry &operator<<(JAry &a, JObj &j) { return a << j.str(); }

struct Ev : public JObj {
    Ev(const wxString &event) {
        double const now = ::wxGetUTCTimeMillis().ToDouble() / 1000.0;
        *this << NV("Event", event) << NV("Timestamp", now, 3)
              << NV("Host", wxGetHostName())
              << NV("Inst", wxGetApp().GetInstanceNumber());
    }
};

static Ev ev_message_version() {
    Ev ev("Version");
    ev << NV("SodiumVersion", SODIUM_VERSION)
       << NV("SodiumSubver", SODIUM_SUBVER) << NV("OverlapSupport", true)
       << NV("MsgVersion", MSG_PROTOCOL_VERSION);
    return ev;
}

static Ev ev_set_lock_position(const SodiumPoint &xy) {
    Ev ev("LockPositionSet");
    ev << xy;
    return ev;
}

static Ev ev_calibration_complete(const Mount *mount) {
    Ev ev("CalibrationComplete");
    ev << NVMount(mount);

    if (mount->IsStepGuider()) {
        ev << NV("Limit", mount->GetAoMaxPos());
    }

    return ev;
}

static Ev ev_star_selected(const SodiumPoint &pos) {
    Ev ev("StarSelected");
    ev << pos;
    return ev;
}

static Ev ev_start_guiding() { return Ev("StartGuiding"); }

static Ev ev_paused() { return Ev("Paused"); }

static Ev ev_start_calibration(const Mount *mount) {
    Ev ev("StartCalibration");
    ev << NVMount(mount);
    return ev;
}

static Ev ev_app_state(EXPOSED_STATE st = Guider::GetExposedState()) {
    Ev ev("AppState");
    ev << NV("State", state_name(st));
    return ev;
}

static Ev ev_settling(double distance, double time, double settleTime,
                      bool starLocked) {
    Ev ev("Settling");

    ev << NV("Distance", distance, 2) << NV("Time", time, 1)
       << NV("SettleTime", settleTime, 1) << NV("StarLocked", starLocked);

    return ev;
}

static Ev ev_settle_done(const wxString &errorMsg, int settleFrames,
                         int droppedFrames) {
    Ev ev("SettleDone");

    int status = errorMsg.IsEmpty() ? 0 : 1;

    ev << NV("Status", status);

    if (status != 0) {
        ev << NV("Error", errorMsg);
    }

    ev << NV("TotalFrames", settleFrames) << NV("DroppedFrames", droppedFrames);

    return ev;
}

struct ClientReadBuf {
    enum { SIZE = 1024 };
    char m_buf[SIZE];
    char *dest;

    ClientReadBuf() { reset(); }
    char *buf() { return &m_buf[0]; }
    size_t len() const { return dest - &m_buf[0]; }
    size_t avail() const { return &m_buf[SIZE] - dest; }
    void reset() { dest = &m_buf[0]; }
};

struct ClientData {
    wxSocketClient *cli;
    int refcnt;
    ClientReadBuf rdbuf;
    wxMutex wrlock;

    ClientData(wxSocketClient *cli_) : cli(cli_), refcnt(1) {}
    void AddRef() { ++refcnt; }
    void RemoveRef() {
        if (--refcnt == 0) {
            cli->Destroy();
            delete this;
        }
    }
};

struct ClientDataGuard {
    ClientData *cd;
    ClientDataGuard(wxSocketClient *cli)
        : cd((ClientData *)cli->GetClientData()) {
        cd->AddRef();
    }
    ~ClientDataGuard() { cd->RemoveRef(); }
    ClientData *operator->() const { return cd; }
};

inline static wxMutex *client_wrlock(wxSocketClient *cli) {
    return &((ClientData *)cli->GetClientData())->wrlock;
}

static wxString SockErrStr(wxSocketError e) {
    switch (e) {
        case wxSOCKET_NOERROR:
            return "";
        case wxSOCKET_INVOP:
            return "Invalid operation";
        case wxSOCKET_IOERR:
            return "Input / Output error";
        case wxSOCKET_INVADDR:
            return "Invalid address";
        case wxSOCKET_INVSOCK:
            return "Invalid socket(uninitialized)";
        case wxSOCKET_NOHOST:
            return "No corresponding host";
        case wxSOCKET_INVPORT:
            return "Invalid port";
        case wxSOCKET_WOULDBLOCK:
            return "operation would block";
        case wxSOCKET_TIMEDOUT:
            return "timeout expired";
        case wxSOCKET_MEMERR:
            return "Memory exhausted";
        default:
            return wxString::Format("unknown socket error %d", e);
    }
}

static void send_buf(wxSocketClient *client, const wxCharBuffer &buf) {
    wxMutexLocker lock(*client_wrlock(client));
    client->Write(buf.data(), buf.length());
    if (client->LastWriteCount() != buf.length()) {
        Debug.Write(wxString::Format(
            "evsrv: cli %p short write %u/%u %s\n", client,
            client->LastWriteCount(), (unsigned int)buf.length(),
            SockErrStr(client->Error() ? client->LastError()
                                       : wxSOCKET_NOERROR)));
    }
}

static void do_notify1(wxSocketClient *client, const JAry &ary) {
    send_buf(client, (JAry(ary).str() + "\r\n").ToUTF8());
}

static void do_notify1(wxSocketClient *client, const JObj &j) {
    send_buf(client, (JObj(j).str() + "\r\n").ToUTF8());
}

static void do_notify(const EventServer::CliSockSet &cli, const JObj &jj) {
    wxCharBuffer buf = (JObj(jj).str() + "\r\n").ToUTF8();

    for (EventServer::CliSockSet::const_iterator it = cli.begin();
         it != cli.end(); ++it) {
        send_buf(*it, buf);
    }
}

inline static void simple_notify(const EventServer::CliSockSet &cli,
                                 const wxString &ev) {
    if (!cli.empty())
        do_notify(cli, Ev(ev));
}

inline static void simple_notify_ev(const EventServer::CliSockSet &cli,
                                    const Ev &ev) {
    if (!cli.empty())
        do_notify(cli, ev);
}

#define SIMPLE_NOTIFY(s) simple_notify(m_eventServerClients, s)
#define SIMPLE_NOTIFY_EV(ev) simple_notify_ev(m_eventServerClients, ev)

/**
 * @brief 向服务器发送应用程序当前状态的通知事件
 *
 * @param cli 指向客户端套接字对象的指针，用于向服务器发送数据
 */
static void send_catchup_events(wxSocketClient *cli) {
    // 获取应用程序的当前暴露状态
    EXPOSED_STATE st = Guider::GetExposedState();

    // 发送消息版本号通知事件
    do_notify1(cli, ev_message_version());

    // 如果当前存在 Guider 对象，则发送锁定位置和当前星位置通知事件
    if (pFrame->pGuider) {
        if (pFrame->pGuider->LockPosition().IsValid())
            do_notify1(cli,
                       ev_set_lock_position(pFrame->pGuider->LockPosition()));

        if (pFrame->pGuider->CurrentPosition().IsValid())
            do_notify1(cli,
                       ev_star_selected(pFrame->pGuider->CurrentPosition()));
    }

    // 如果已经完成了主天文望远镜的校准，则发送校准完成通知事件
    if (pMount && pMount->IsCalibrated())
        do_notify1(cli, ev_calibration_complete(pMount));

    // 如果已经完成了辅助天文望远镜的校准，则发送校准完成通知事件
    if (pSecondaryMount && pSecondaryMount->IsCalibrated())
        do_notify1(cli, ev_calibration_complete(pSecondaryMount));

    // 根据应用程序的当前状态发送相应的通知事件
    if (st == EXPOSED_STATE_GUIDING_LOCKED) {
        do_notify1(cli, ev_start_guiding());
    } else if (st == EXPOSED_STATE_CALIBRATING) {
        Mount *mount = pMount;
        if (pFrame->pGuider->GetState() == STATE_CALIBRATING_SECONDARY)
            mount = pSecondaryMount;
        do_notify1(cli, ev_start_calibration(mount));
    } else if (st == EXPOSED_STATE_PAUSED) {
        do_notify1(cli, ev_paused());
    }

    // 发送应用程序当前状态通知事件
    do_notify1(cli, ev_app_state());
}

static void destroy_client(wxSocketClient *cli) {
    ClientData *buf = (ClientData *)cli->GetClientData();
    buf->RemoveRef();
}

static void drain_input(wxSocketInputStream &sis) {
    while (sis.CanRead()) {
        char buf[1024];
        if (sis.Read(buf, sizeof(buf)).LastRead() == 0)
            break;
    }
}

enum {
    JSONRPC_PARSE_ERROR = -32700,
    JSONRPC_INVALID_REQUEST = -32600,
    JSONRPC_METHOD_NOT_FOUND = -32601,
    JSONRPC_INVALID_PARAMS = -32602,
    JSONRPC_INTERNAL_ERROR = -32603,
};

static NV jrpc_error(int code, const wxString &msg) {
    JObj err;
    err << NV("code", code) << NV("message", msg);
    return NV("error", err);
}

template <typename T>
static NV jrpc_result(const T &t) {
    return NV("result", t);
}

template <typename T>
static NV jrpc_result(T &t) {
    return NV("result", t);
}

static NV jrpc_id(const json_value *id) { return NV("id", id); }

struct JRpcResponse : public JObj {
    JRpcResponse() { *this << NV("jsonrpc", "2.0"); }
};

static wxString parser_error(const JsonParser &parser) {
    spdlog::error("Failed to parser JSON message from client...");
    spdlog::debug("invalid JSON request: {} on line {} at \"{}...\"",
                  parser.ErrorDesc(), parser.ErrorLine(), parser.ErrorPos());
    return wxString::Format(
        "invalid JSON request: %s on line %d at \"%.12s...\"",
        parser.ErrorDesc(), parser.ErrorLine(), parser.ErrorPos());
}

/**
 * @brief 解析 JSON 对象，提取其中的方法、参数以及请求 ID
 *
 * @param req 指向要解析的 JSON 对象的指针
 * @param pmethod 指向方法名的指针，如果解析成功，则存储方法对象
 * @param pparams 指向参数列表的指针，如果解析成功，则存储参数对象
 * @param pid 指向请求 ID 的指针，如果解析成功，则存储请求 ID 对象
 */
static void parse_request(const json_value *req, const json_value **pmethod,
                          const json_value **pparams, const json_value **pid) {
    // 初始化输出结果的指针变量
    *pmethod = *pparams = *pid = 0;

    // 如果输入的 JSON 对象存在，则遍历其所有成员
    if (req) {
        json_for_each(t, req) {
            if (t->name) {
                if (t->type == JSON_STRING && strcmp(t->name, "method") == 0)
                    // 如果成员名称为 "method"，则将其保存到 *pmethod
                    // 指向的变量中
                    *pmethod = t;
                else if (strcmp(t->name, "params") == 0)
                    // 如果成员名称为 "params"，则将其保存到 *pparams
                    // 指向的变量中
                    *pparams = t;
                else if (strcmp(t->name, "id") == 0)
                    // 如果成员名称为 "id"，则将其保存到 *pid 指向的变量中
                    *pid = t;
            }
        }
    }
}

// paranoia
#define VERIFY_GUIDER(response)                          \
    do {                                                 \
        if (!pFrame || !pFrame->pGuider) {               \
            response << jrpc_error(1, "internal error"); \
            return;                                      \
        }                                                \
    } while (0)

/**
 * @brief 取消当前的星选中状态
 *
 * @param response 存储响应结果的 JObj 对象
 * @param params 代表参数的 JSON 对象的指针，本函数没有使用
 */
static void deselect_star(JObj &response, const json_value *params) {
    VERIFY_GUIDER(response);  // 确认当前引导器可用（宏定义）

    try {
        pFrame->pGuider->Reset(true);  // 重置当前引导器，取消星的选中状态
        spdlog::debug("Reset star selection");  // 打印调试信息
        response << jrpc_result(0);  // 返回响应结果为成功（值为 0）的 JSON 对象
    } catch (const std::exception &e) {  // 异常捕获
        spdlog::error("Error occurred in deselect_star: {}",
                      e.what());              // 打印错误信息
        response << jrpc_error(1, e.what());  // 返回响应结果为错误（代号为
                                              // 1），并包含错误信息的 JSON 对象
    }
}

/**
 * @brief 获取当前曝光时间
 *
 * @param response 存储响应结果的 JObj 对象
 * @param params 代表参数的 JSON 对象的指针，本函数没有使用
 */
static void get_exposure(JObj &response, const json_value *params) {
    try {
        response << jrpc_result(
            pFrame
                ->RequestedExposureDuration());  // 返回响应结果为当前曝光时间的
                                                 // JSON 对象
        spdlog::debug("Current Exposure time : {}",
                      pFrame->RequestedExposureDuration());  // 打印调试信息
    } catch (const std::exception &e) {                      // 异常捕获
        spdlog::error("Error occurred in get_exposure: {}",
                      e.what());              // 打印错误信息
        response << jrpc_error(1, e.what());  // 返回响应结果为错误（代号为
                                              // 1），并包含错误信息的 JSON 对象
    }
}

/**
 * @brief 获取当前可用的曝光时间列表
 *
 * @param response 存储响应结果的 JObj 对象
 * @param params 代表参数的 JSON 对象的指针，本函数没有使用
 */
static void get_exposure_durations(JObj &response, const json_value *params) {
    try {
        const std::vector<int> &exposure_durations =
            pFrame->GetExposureDurations();  // 获取当前可用的曝光时间列表
        response << jrpc_result(
            exposure_durations);  // 返回响应结果为曝光时间列表的 JSON 对象
    } catch (const std::exception &e) {  // 异常捕获
        spdlog::error("Error occurred in get_exposure_durations: {}",
                      e.what());              // 打印错误信息
        response << jrpc_error(1, e.what());  // 返回响应结果为错误（代号为
                                              // 1），并包含错误信息的 JSON 对象
    }
}

/**
 * @brief 获取所有可用的配置文件列表
 *
 * @param response 存储响应结果的 JObj 对象
 * @param params 代表参数的 JSON 对象的指针，本函数没有使用
 */
static void get_profiles(JObj &response,
                         [[maybe_unused]] const json_value *params) {
    JAry ary;  // 定义一个空的 JSON 数组，用于存储所有配置文件信息
    try {
        wxArrayString names =
            pConfig->ProfileNames();  // 获取所有可用的配置文件名
        for (const auto &name : names) {
            // 获取当前配置文件名
            int id = pConfig->GetProfileId(name);  // 获取当前配置文件 ID
            if (id != 0) {  // 如果 ID 非零，则表示该配置文件可用
                JObj t;  // 定义一个空的 JSON 对象，用于存储当前配置文件信息
                // 向 t 中添加表示配置文件 ID、名称和是否被选中等信息的键值对
                t << NV("id", id) << NV("name", name);
                if (id == pConfig->GetCurrentProfileId()) {
                    t << NV("selected", true);
                }
                ary << t;  // 将当前配置文件信息添加到数组中
            }
        }
        response << jrpc_result(ary);  // 返回响应结果为配置文件列表的 JSON 对象
    } catch (const std::exception &e) {  // 异常捕获
        spdlog::error("Error occurred in get_profiles: {}",
                      e.what());              // 打印错误信息
        response << jrpc_error(1, e.what());  // 返回响应结果为错误（代号为
                                              // 1），并包含错误信息的 JSON 对象
    }
}

struct Params {
    std::map<std::string, const json_value *> dict;

    void Init(const char *names[], size_t nr_names, const json_value *params) {
        if (params == nullptr) {
            return;
        }
        if (params->type == JSON_ARRAY) {
            const json_value *jv = params->first_child;
            for (size_t i = 0; (jv != nullptr) && i < nr_names;
                 i++, jv = jv->next_sibling) {
                const char *name = names[i];
                dict.insert(std::make_pair(std::string(name), jv));
            }
        } else if (params->type == JSON_OBJECT) {
            json_for_each(jv, params) {
                dict.insert(std::make_pair(std::string(jv->name), jv));
            }
        }
    }
    // Max: 参数数量暴力处理
    Params(const char *n1, const json_value *params) {
        const char *n[] = {n1};
        Init(n, 1, params);
    }
    Params(const char *n1, const char *n2, const json_value *params) {
        const char *n[] = {n1, n2};
        Init(n, 2, params);
    }
    Params(const char *n1, const char *n2, const char *n3,
           const json_value *params) {
        const char *n[] = {n1, n2, n3};
        Init(n, 3, params);
    }
    Params(const char *n1, const char *n2, const char *n3, const char *n4,
           const json_value *params) {
        const char *n[] = {n1, n2, n3, n4};
        Init(n, 4, params);
    }
    Params(const char *n1, const char *n2, const char *n3, const char *n4,
           const char *n5, const json_value *params) {
        const char *n[] = {n1, n2, n3, n4, n5};
        Init(n, 5, params);
    }
    Params(const char *n1, const char *n2, const char *n3, const char *n4,
           const char *n5, const char *n6, const json_value *params) {
        const char *n[] = {n1, n2, n3, n4, n5, n6};
        Init(n, 6, params);
    }
    Params(const char *n1, const char *n2, const char *n3, const char *n4,
           const char *n5, const char *n6, const char *n7,
           const json_value *params) {
        const char *n[] = {n1, n2, n3, n4, n5, n6, n7};
        Init(n, 7, params);
    }
    [[nodiscard]] auto param(const std::string &name) const
        -> const json_value * {
        auto it = dict.find(name);
        return it == dict.end() ? 0 : it->second;
    }
};

static void set_exposure(JObj &response, const json_value *params) {
    try {
        Params p("exposure", params);
        const json_value *exp = p.param("exposure");
        spdlog::debug("Try setting the exposure time to {}", exp->float_value);
        if (!exp || exp->type != JSON_INT) {
            spdlog::debug("Exposure time parameter not found");
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected exposure param");
            return;
        }
        bool ok = pFrame->SetExposureDuration(exp->int_value);
        if (ok) {
            spdlog::debug("Successfully set exposure time");
            response << jrpc_result(0);
        } else {
            spdlog::debug("Failed to set exposure time");
            response << jrpc_error(1, "could not set exposure duration");
        }
    } catch (std::exception &e) {
        response << jrpc_error(1,
                               "exception caught while setting exposure time");
        spdlog::error("Exception caught while setting exposure time: {}",
                      e.what());
    } catch (...) {
        response << jrpc_error(
            1, "unknown exception caught while setting exposure time");
        spdlog::error("Unknown exception caught while setting exposure time");
    }
}

static void get_profile(JObj &response, const json_value *params) {
    try {
        int id = pConfig->GetCurrentProfileId();
        wxString name = pConfig->GetCurrentProfile();
        spdlog::debug("Current profile name : {}", std::string(name));
        JObj t;
        t << NV("id", id) << NV("name", name);
        response << jrpc_result(t);
    } catch (std::exception &e) {
        response << jrpc_error(
            1, "exception caught while getting current profile");
        spdlog::error("Exception caught while getting current profile: {}",
                      e.what());
    } catch (...) {
        response << jrpc_error(
            1, "unknown exception caught while getting current profile");
        spdlog::error("Unknown exception caught while getting current profile");
    }
}

inline static void devstat(JObj &t, const char *dev, const wxString &name,
                           bool connected) {
    try {
        JObj o;
        t << NV(dev, o << NV("name", name) << NV("connected", connected));
    } catch (std::exception &e) {
        spdlog::error("Exception caught while setting device status: {}",
                      e.what());
    } catch (...) {
        spdlog::error("Unknown exception caught while setting device status");
    }
}

static void get_current_equipment(JObj &response,
                                  [[maybe_unused]] const json_value *params) {
    spdlog::debug("Try to get all of the currently connected devices");

    JObj t;

    if (pCamera != nullptr) {
        devstat(t, "camera", pCamera->Name, pCamera->Connected);
    }
    spdlog::debug("Camera name : {} , connect : {}", std::string(pCamera->Name),
                  pCamera->Connected);

    Mount *mount = TheScope();
    if (mount != nullptr) {
        devstat(t, "mount", mount->Name(), mount->IsConnected());
    }
    spdlog::debug("Telescope name : {} , connect : {}",
                  std::string(pMount->Name()), pMount->IsConnected());

    Mount *auxMount = pFrame->pGearDialog->AuxScope();
    if (auxMount != nullptr) {
        devstat(t, "aux_mount", auxMount->Name(), auxMount->IsConnected());
    }

    Mount *ao = TheAO();
    if (ao != nullptr) {
        devstat(t, "AO", ao->Name(), ao->IsConnected());
    }

    Rotator *rotator = pRotator;
    if (rotator != nullptr) {
        devstat(t, "rotator", rotator->Name(), rotator->IsConnected());
    }

    response << jrpc_result(t);
}

/// @brief
/// @return status(bool)
static auto all_equipment_connected() -> bool {
    try {
        spdlog::debug("Check that all devices are connected");
        bool status =
            (pCamera != nullptr) && pCamera->Connected &&
            ((pMount == nullptr) || pMount->IsConnected()) &&
            ((pSecondaryMount == nullptr) || pSecondaryMount->IsConnected());
        if (status) {
            spdlog::debug("All of the devices connected");
        } else {
            spdlog::debug("Not all of the devices connected");
        }
        return status;
    } catch (std::exception &e) {
        spdlog::error("Exception caught in all_equipment_connected: {}",
                      e.what());
        return false;
    }
}

/// @brief Set the current file according to the configuration file number. This
/// configuration should already exist.
/// @param response
/// @param params
static void set_profile(JObj &response, const json_value *params) {
    try {
        spdlog::debug("Trying to set current profile");
        Params p("id", params);
        const json_value *id = p.param("id");
        if (!id || id->type != JSON_INT) {
            spdlog::error("Profile number not found in parameters");
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected profile id param");
            return;
        }
        VERIFY_GUIDER(response);
        wxString errMsg;
        bool error = pFrame->pGearDialog->SetProfile(id->int_value, &errMsg);
        if (error) {
            spdlog::error("Failed to set profile");
            response << jrpc_error(1, errMsg);
        } else {
            spdlog::debug("Set profile succesfully");
            response << jrpc_result(0);
        }
    } catch (std::exception &e) {
        spdlog::error("Exception caught in set_profile: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void get_connected(JObj &response,
                          [[maybe_unused]] const json_value *params) {
    response << jrpc_result(all_equipment_connected());
}

// 连接所有设备
static void set_connected(JObj &response, const json_value *params) {
    try {
        Params p("connected", params);
        const json_value *val = p.param("connected");
        if ((val == nullptr) || val->type != JSON_BOOL) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected connected boolean param");
            return;
        }
        VERIFY_GUIDER(response);
        bool connect = val->int_value != 0;
        std::string action = connect ? "connect" : "disconnect";
        spdlog::debug("Try to {} all devices", action);
        wxString errMsg;
        spdlog::info(
            "Device dialog is {}",
            pFrame->pGearDialog->IsShownOnScreen() ? "active" : "inactive");
        if (pFrame->pGearDialog->IsShown()) {
            spdlog::warn("Device dialog is active, we will try to kill it!");
            if ((pFrame->pGearDialog->Close(true))) {
                spdlog::debug("Device dialog closed");
            } else {
                spdlog::error("Failed to kill device dialog");
                response << jrpc_error(1, "Failed to kill device dialog");
                return;
            }
        }
        bool error = connect ? pFrame->pGearDialog->ConnectAll(&errMsg)
                             : pFrame->pGearDialog->DisconnectAll(&errMsg);
        if (error) {
            spdlog::error("Failed to {} to devices , error : {}", action,
                          std::string(errMsg));
            response << jrpc_error(1, errMsg);
        } else {
            spdlog::debug("Successfully {} to devices", action);
            response << jrpc_result(0);
        }
    } catch (std::exception &e) {
        spdlog::error("Exception caught in set_connected: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

// 检查是否已完成校准
static void get_calibrated(JObj &response,
                           [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Check if the calibration has been completed");
        bool calibrated =
            (pMount != nullptr) && pMount->IsCalibrated() &&
            ((pSecondaryMount == nullptr) || pSecondaryMount->IsCalibrated());
        if (calibrated) {
            spdlog::debug(
                "Calibration had already completed, you can start guiding now");
        } else {
            spdlog::debug("Calibration had not been completed.");
        }
        response << jrpc_result(calibrated);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_calibrated: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static auto float_param(const json_value *v, double *p) -> bool {
    if (v->type == JSON_INT) {
        *p = (double)v->int_value;
        return true;
    }
    if (v->type == JSON_FLOAT) {
        *p = v->float_value;
        return true;
    }

    return false;
}

static auto float_param(const char *name, const json_value *v,
                        double *p) -> bool {
    if (strcmp(name, v->name) != 0) {
        return false;
    }

    return float_param(v, p);
}

inline static auto bool_value(const json_value *v) -> bool {
    return v->int_value != 0;
}

static auto bool_param(const json_value *jv, bool *val) -> bool {
    if (jv->type != JSON_BOOL && jv->type != JSON_INT) {
        return false;
    }
    *val = bool_value(jv);
    return true;
}

static void get_paused(JObj &response,
                       [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Check whether the guide star has paused");
        VERIFY_GUIDER(response);
        bool status = pFrame->pGuider->IsPaused();
        if (status) {
            spdlog::debug("Sodium paused successfully");
        } else {
            spdlog::debug("Sodium is not paused");
        }
        response << jrpc_result(status);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_paused: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}
// 设置暂停
static void set_paused(JObj &response, const json_value *params) {
    try {
        spdlog::debug("Try to pause the current work");
        Params p("paused", "type", params);
        const json_value *jv = p.param("paused");
        bool val;
        if (!jv || !bool_param(jv, &val)) {
            spdlog::error("Unexpected params given by client when set paused");
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected bool param at index 0");
            return;
        }
        PauseType pause = PAUSE_NONE;
        if (val) {
            pause = PAUSE_GUIDING;
            jv = p.param("type");
            if (jv != nullptr) {
                if (jv->type == JSON_STRING) {
                    if (strcmp(jv->string_value, "full") == 0) {
                        pause = PAUSE_FULL;
                    }
                } else {
                    response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                           "expected string param at index 1");
                    return;
                }
            }
        }
        pFrame->SetPaused(pause);
        spdlog::debug("The current task was suspended successfully");
        response << jrpc_result(0);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in set_paused: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void loop(JObj &response, [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Trying to start looping...");
        bool error = pFrame->StartLooping();
        if (error) {
            spdlog::error("Failed to start looping!");
            response << jrpc_error(1, "could not start looping");
        } else {
            spdlog::debug("Started looping successfully");
            response << jrpc_result(0);
        }
    } catch (std::exception &e) {
        spdlog::error("Exception caught in loop: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void stop_capture(JObj &response,
                         [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Stop capture...");
        pFrame->StopCapturing();
        response << jrpc_result(0);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in stop_capture: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static auto parse_rect(wxRect *r, const json_value *j) -> bool {
    if (j->type != JSON_ARRAY) {
        return false;
    }

    int a[4];
    const json_value *jv = j->first_child;
    for (int &i : a) {
        if ((jv == nullptr) || jv->type != JSON_INT) {
            return false;
        }
        i = jv->int_value;
        jv = jv->next_sibling;
    }
    if (jv != nullptr) {
        return false;  // extra value
    }

    r->x = a[0];
    r->y = a[1];
    r->width = a[2];
    r->height = a[3];

    return true;
}

// 寻找星星
static void find_star(JObj &response, const json_value *params) {
    try {
        VERIFY_GUIDER(response);
        spdlog::debug("Trying to find a star for guiding...");
        Params p("roi", params);
        wxRect roi;
        const json_value *j = p.param("roi");
        if ((j != nullptr) && !parse_rect(&roi, j)) {
            spdlog::error("Invalid ROI parameters");
            response << jrpc_error(JSONRPC_INVALID_PARAMS, "invalid ROI param");
            return;
        }
        bool error = pFrame->AutoSelectStar(roi);
        if (!error) {
            const SodiumPoint &lockPos = pFrame->pGuider->LockPosition();
            if (lockPos.IsValid()) {
                spdlog::debug("Found a star at [{}, {}]", lockPos.X, lockPos.Y);
                response << jrpc_result(lockPos);
                return;
            }
        }
        spdlog::error("Failed to find a good star for guiding");
        response << jrpc_error(1, "could not find star");
    } catch (std::exception &e) {
        spdlog::error("Exception caught in find_star: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void get_pixel_scale(JObj &response,
                            [[maybe_unused]] const json_value *params) {
    try {
        double scale = pFrame->GetCameraPixelScale();
        if (scale == 1.0) {
            response << jrpc_result(NULL_VALUE);  // scale unknown
        } else {
            spdlog::debug("Current pixel size: {}", scale);
            response << jrpc_result(scale);
        }
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_pixel_scale: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void get_app_state(JObj &response,
                          [[maybe_unused]] const json_value *params) {
    try {
        EXPOSED_STATE st = Guider::GetExposedState();
        response << jrpc_result(state_name(st));
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_app_state: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void get_lock_position(JObj &response,
                              [[maybe_unused]] const json_value *params) {
    try {
        VERIFY_GUIDER(response);
        const SodiumPoint &lockPos = pFrame->pGuider->LockPosition();
        if (lockPos.IsValid()) {
            response << jrpc_result(lockPos);
        } else {
            response << jrpc_result(NULL_VALUE);
        }
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_lock_position: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

// {"method": "set_lock_position", "params": [X, Y, true], "id": 1}
// 设置锁定位置
static void set_lock_position(JObj &response, const json_value *params) {
    try {
        // 解析参数
        Params p("x", "y", "exact", params);
        const json_value *p0 = p.param("x");
        const json_value *p1 = p.param("y");
        double x;
        double y;
        if ((p0 == nullptr) || (p1 == nullptr) || !float_param(p0, &x) ||
            !float_param(p1, &y)) {
            spdlog::error("Invalid parameters for set_lock_position");
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected lock position x, y params");
            return;
        }
        bool exact = true;
        const json_value *p2 = p.param("exact");
        if (p2 != nullptr) {
            if (!bool_param(p2, &exact)) {
                spdlog::error("Invalid parameters for set_lock_position");
                response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                       "expected boolean param at index 2");
                return;
            }
        }
        // 验证导星器
        VERIFY_GUIDER(response);
        bool error;
        // 设置锁定位置
        if (exact) {
            error = pFrame->pGuider->SetLockPosition(SodiumPoint(x, y));
        } else {
            error =
                pFrame->pGuider->SetLockPosToStarAtPosition(SodiumPoint(x, y));
        }
        if (error) {
            spdlog::error("Failed to set lock position");
            response << jrpc_error(JSONRPC_INVALID_REQUEST,
                                   "could not set lock position");
            return;
        }
        spdlog::debug("Lock position set to [{}, {}]", x, y);
        response << jrpc_result(0);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in set_lock_position: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

inline static auto string_val(const json_value *j) -> const char * {
    return j->type == JSON_STRING ? j->string_value : "";
}

enum WHICH_MOUNT { MOUNT, AO, WHICH_MOUNT_BOTH, WHICH_MOUNT_ERR };

static auto which_mount(const json_value *p) -> WHICH_MOUNT {
    WHICH_MOUNT r = MOUNT;
    if (p) {
        r = WHICH_MOUNT_ERR;
        if (p->type == JSON_STRING) {
            if (wxStricmp(p->string_value, "ao") == 0)
                r = AO;
            else if (wxStricmp(p->string_value, "mount") == 0)
                r = MOUNT;
            else if (wxStricmp(p->string_value, "both") == 0)
                r = WHICH_MOUNT_BOTH;
        }
    }
    return r;
}

static void clear_calibration(JObj &response, const json_value *params) {
    bool clear_mount;
    bool clear_ao;
    spdlog::debug("Try to clear all calibration models");
    try {
        // 解析参数
        if (params == nullptr) {
            clear_mount = clear_ao = true;
        } else {
            Params p("which", params);
            clear_mount = clear_ao = false;
            WHICH_MOUNT which = which_mount(p.param("which"));
            switch (which) {
                case MOUNT:
                    clear_mount = true;
                    break;
                case AO:
                    clear_ao = true;
                    break;
                case WHICH_MOUNT_BOTH:
                    clear_mount = clear_ao = true;
                    break;
                case WHICH_MOUNT_ERR:
                    response << jrpc_error(
                        JSONRPC_INVALID_PARAMS,
                        R"(expected param "mount", "ao", or "both")");
                    return;
            }
        }
        // 清除校准模型
        Mount *mount = TheScope();
        Mount *ao = TheAO();
        if ((mount != nullptr) && clear_mount) {
            mount->ClearCalibration();
            spdlog::debug("Successfully cleared mount calibration model");
        }
        if ((ao != nullptr) && clear_ao) {
            ao->ClearCalibration();
            spdlog::debug("Successfully cleared AO calibration model");
        }
        response << jrpc_result(0);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
        spdlog::error("Error occurred while clearing calibration models: {}",
                      e.what());
    }
}

static void flip_calibration(JObj &response,
                             [[maybe_unused]] const json_value *params) {
    try {
        // 翻转校准数据
        bool error = pFrame->FlipCalibrationData();
        if (error) {
            response << jrpc_error(1, "could not flip calibration");
            spdlog::error("Error occurred while flipping calibration data");
        } else {
            response << jrpc_result(0);
            spdlog::debug("Successfully flipped calibration data");
        }
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
        spdlog::error("Error occurred while flipping calibration data: {}",
                      e.what());
    }
}

static void get_lock_shift_enabled(JObj &response,
                                   [[maybe_unused]] const json_value *params) {
    try {
        // 获取锁定位置偏移参数
        VERIFY_GUIDER(response);
        bool enabled = pFrame->pGuider->GetLockPosShiftParams().shiftEnabled;
        response << jrpc_result(enabled);
        spdlog::debug("Successfully got lock position shift enabled parameter");
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
        spdlog::error(
            "Error occurred while getting lock position shift enabled "
            "parameter: {}",
            e.what());
    }
}

static void set_lock_shift_enabled(JObj &response, const json_value *params) {
    try {
        // 设置锁定位置偏移参数
        Params p("enabled", params);
        const json_value *val = p.param("enabled");
        bool enable;
        if ((val == nullptr) || !bool_param(val, &enable)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected enabled boolean param");
            return;
        }
        VERIFY_GUIDER(response);
        pFrame->pGuider->EnableLockPosShift(enable);
        response << jrpc_result(0);
        spdlog::debug("Successfully set lock position shift enabled parameter");
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
        spdlog::error(
            "Error occurred while setting lock position shift enabled "
            "parameter: {}",
            e.what());
    }
}

/// @brief
/// @param params
/// @return
// 判断是否需要相机移动请求
static auto is_camera_shift_req(const json_value *params) -> bool {
    Params p("axes", params);
    const json_value *j = p.param("axes");
    if (j) {
        const char *axes = string_val(j);
        if (wxStricmp(axes, "x/y") == 0 || wxStricmp(axes, "camera") == 0) {
            return true;
        }
    }
    return false;
}

// 序列化 LockPosShiftParams 对象为 JObj 对象
static auto operator<<(JObj &j, const LockPosShiftParams &l) -> JObj & {
    j << NV("enabled", l.shiftEnabled);
    if (l.shiftRate.IsValid()) {
        j << NV("rate", l.shiftRate)
          << NV("units",
                l.shiftUnits == UNIT_ARCSEC ? "arcsec/hr" : "pixels/hr")
          << NV("axes", l.shiftIsMountCoords ? "RA/Dec" : "X/Y");
    }
    return j;
}

// 获取 LockPosShiftParams 对象
static void get_lock_shift_params(JObj &response, const json_value *params) {
    VERIFY_GUIDER(response);
    try {
        // 获取 LockPosShiftParams 对象
        const LockPosShiftParams &lockShift =
            pFrame->pGuider->GetLockPosShiftParams();
        JObj rslt;
        if (is_camera_shift_req(params)) {
            // 如果是相机移动请求，构造临时的 LockPosShiftParams 对象
            LockPosShiftParams tmp;
            tmp.shiftEnabled = lockShift.shiftEnabled;
            const ShiftPoint &lock = pFrame->pGuider->LockPosition();
            tmp.shiftRate = lock.ShiftRate() * 3600;  // px/sec => px/hr
            tmp.shiftUnits = UNIT_PIXELS;
            tmp.shiftIsMountCoords = false;
            rslt << tmp;
        } else {
            // 否则直接使用 LockPosShiftParams 对象
            rslt << lockShift;
        }
        response << jrpc_result(rslt);
    } catch (std::exception &e) {
        // 异常捕获，输出错误信息
        Debug.Write(wxString::Format("get_lock_shift_params exception: %s\n",
                                     e.what()));
        response << jrpc_error(-32603, "Internal error");
    }
}

static auto get_double(double *d, const json_value *j) -> bool {
    if (j->type == JSON_FLOAT) {
        *d = j->float_value;
        return true;
    }
    if (j->type == JSON_INT) {
        *d = j->int_value;
        return true;
    }
    return false;
}

static auto parse_point(SodiumPoint *pt, const json_value *j) -> bool {
    if (j->type != JSON_ARRAY) {
        return false;
    }
    const json_value *jx = j->first_child;
    if (jx == nullptr) {
        return false;
    }
    const json_value *jy = jx->next_sibling;
    if ((jy == nullptr) || (jy->next_sibling != nullptr)) {
        return false;
    }
    double x;
    double y;
    if (!get_double(&x, jx) || !get_double(&y, jy)) {
        return false;
    }
    pt->SetXY(x, y);
    return true;
}

// 解析锁定位置移动参数
static auto parse_lock_shift_params(LockPosShiftParams *shift,
                                    const json_value *params,
                                    wxString *error) -> bool {
    // 锁定位置移动参数格式为：
    // {"rate":[3.3,1.1],"units":"arcsec/hr","axes":"RA/Dec"}
    // 如果传入的参数是数组形式，则取数组的第一个元素
    if ((params != nullptr) && params->type == JSON_ARRAY) {
        params = params->first_child;
    }
    // 解析参数
    Params p("rate", "units", "axes", params);
    // 设置默认值
    shift->shiftUnits = UNIT_ARCSEC;
    shift->shiftIsMountCoords = true;
    const json_value *j;
    // 解析速率值
    j = p.param("rate");
    if (!j || !parse_point(&shift->shiftRate, j)) {
        *error = "expected rate value array";
        return false;
    }
    // 解析单位
    j = p.param("units");
    const char *units = j ? string_val(j) : "";
    if (wxStricmp(units, "arcsec/hr") == 0 ||
        wxStricmp(units, "arc-sec/hr") == 0) {
        shift->shiftUnits = UNIT_ARCSEC;
    } else if (wxStricmp(units, "pixels/hr") == 0) {
        shift->shiftUnits = UNIT_PIXELS;
    } else {
        *error = "expected units 'arcsec/hr' or 'pixels/hr'";
        return false;
    }
    // 解析坐标轴类型
    j = p.param("axes");
    const char *axes = j ? string_val(j) : "";
    if (wxStricmp(axes, "RA/Dec") == 0) {
        shift->shiftIsMountCoords = true;
    } else if (wxStricmp(axes, "X/Y") == 0) {
        shift->shiftIsMountCoords = false;
    } else {
        *error = "expected axes 'RA/Dec' or 'X/Y'";
        return false;
    }
    return true;
}

// 设置锁定位置移动参数
static void set_lock_shift_params(JObj &response, const json_value *params) {
    // 解析参数
    wxString err;
    LockPosShiftParams shift;
    if (!parse_lock_shift_params(&shift, params, &err)) {
        response << jrpc_error(JSONRPC_INVALID_PARAMS, err);
        return;
    }
    // 验证引导器是否存在
    VERIFY_GUIDER(response);
    // 设置锁定位置移动参数
    try {
        pFrame->pGuider->SetLockPosShiftRate(shift.shiftRate, shift.shiftUnits,
                                             shift.shiftIsMountCoords, true);
    } catch (const wxString &msg) {
        // 异常捕获
        response << jrpc_error(1, msg);
        return;
    }
    response << jrpc_result(0);
}

// 保存图像到本地文件
static void save_image(JObj &response, const json_value *params) {
    // 调试输出
    spdlog::debug("Try to save the image locally");
    // 验证引导器是否存在
    VERIFY_GUIDER(response);
    // 检查是否有可用的图像
    if (pFrame->pGuider->CurrentImage()->ImageData == nullptr) {
        spdlog::error("No image available");
        response << jrpc_error(2, "no image available");
        return;
    }
    // 创建临时文件名
    wxString fname = wxFileName::CreateTempFileName(
        MyFrame::GetDefaultFileDir() + PATHSEPSTR + "save_image_");
    // 保存图像到本地文件
    try {
        if (pFrame->pGuider->SaveCurrentImage(fname)) {
            ::wxRemove(fname);
            spdlog::error("Failed to save an image");
            response << jrpc_error(3, "error saving image");
            return;
        }
    } catch (const wxString &msg) {
        // 异常捕获
        response << jrpc_error(1, msg);
        return;
    }
    // 返回结果
    JObj rslt;
    rslt << NV("filename", fname);
    response << jrpc_result(rslt);
}

/// @brief Capture a single image , not loop
/// @param response
/// @param params {"exposure" : int}
/**
 * @brief Capture a single frame with optional exposure time and subframe
 * parameters
 * @param response JSON-RPC response object
 * @param params JSON-RPC parameters object
 */
static void capture_single_frame(JObj &response, const json_value *params) {
    try {
        // Check if capture is already active
        if (pFrame->CaptureActive) {
            spdlog::error(
                "Cannot capture single frame when capture is currently active");
            response << jrpc_error(
                1,
                "Cannot capture single frame when capture is currently active");
            return;
        }
        // Parse exposure parameter or use default
        Params p("exposure", "subframe", params);
        const json_value *j = p.param("exposure");
        int exposure;
        if (j) {
            if (j->type != JSON_INT || j->int_value < 1 ||
                j->int_value > 10 * 60000) {
                response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                       "Expected exposure parameter to be an "
                                       "integer between 1 and 600000");
                return;
            }
            exposure = j->int_value;
        } else {
            exposure = pFrame->RequestedExposureDuration();
        }
        // Parse subframe parameter or use full frame
        wxRect subframe;
        if ((j = p.param("subframe")) != nullptr) {
            if (!parse_rect(&subframe, j)) {
                response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                       "Invalid subframe parameter");
                return;
            }
        } else {
            subframe = wxRect(0, 0, pCamera->FullSize.GetWidth(),
                              pCamera->FullSize.GetHeight());
        }
        // Start single exposure with given parameters
        bool err = pFrame->StartSingleExposure(exposure, subframe);
        if (err) {
            spdlog::error("Failed to start exposure");
            response << jrpc_error(2, "Failed to start exposure");
            return;
        }
        spdlog::debug("Captured a single image successfully");
        response << jrpc_result(0);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in capture_single_frame: {}", e.what());
        response << jrpc_error(
            JSONRPC_INTERNAL_ERROR,
            "Internal error occurred in capture_single_frame");
    }
}

/**
 * @brief Check whether the camera uses sub-frame
 * @param response JSON-RPC response object
 * @param params JSON-RPC parameters object
 */
static void get_use_subframes(JObj &response,
                              [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Checking if camera uses sub-frames");
        response << jrpc_result(pCamera && pCamera->UseSubframes);
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_use_subframes: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR,
                               "Internal error occurred in get_use_subframes");
    }
}

/**
 * @brief Get the frame size used by the current camera
 * @param response JSON-RPC response object
 * @param params JSON-RPC parameters object
 */
static void get_search_region(JObj &response,
                              [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Getting frame size used by current camera");
        VERIFY_GUIDER(response);
        response << jrpc_result(pFrame->pGuider->GetSearchRegion());
    } catch (std::exception &e) {
        spdlog::error("Exception caught in get_search_region: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR,
                               "Internal error occurred in get_search_region");
    }
}

struct B64Encode {
    static const char *const E;
    std::ostringstream os;
    unsigned int t;
    size_t nread;

    B64Encode() : t(0), nread(0) {}
    void append1(unsigned char ch) {
        t <<= 8;
        t |= ch;
        if (++nread % 3 == 0) {
            os << E[t >> 18] << E[(t >> 12) & 0x3F] << E[(t >> 6) & 0x3F]
               << E[t & 0x3F];
            t = 0;
        }
    }
    void append(const void *src_, size_t len) {
        const auto *src = (const unsigned char *)src_;
        const unsigned char *const end = src + len;
        while (src < end) {
            append1(*src++);
        }
    }
    auto finish() -> std::string {
        switch (nread % 3) {
            case 1:
                os << E[t >> 2] << E[(t & 0x3) << 4] << "==";
                break;
            case 2:
                os << E[t >> 10] << E[(t >> 4) & 0x3F] << E[(t & 0xf) << 2]
                   << '=';
                break;
        }
        os << std::ends;
        return os.str();
    }
};
const char *const B64Encode::E =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief 从当前图像中获取一个星星的图像，并将其转换为base64编码格式返回给客户端
 *
 * @param response JRPC响应对象，用于返回结果或错误信息
 * @param params 包含请求参数的JSON值
 *
 * 请求参数：
 *   - size: 可选，图像大小，必须为整数类型，默认为15，范围为[15, INT_MAX]
 *
 * 响应结果：
 *   - frame: 当前图像的帧序号
 *   - width: 图像宽度
 *   - height: 图像高度
 *   - star_pos: 星星在图像中的位置
 *   - pixels: base64编码格式的星星图像数据
 *
 * 错误码：
 *   - JSONRPC_INVALID_PARAMS(0): 无效的请求参数
 *   - 2: 未选定星星
 */
static void get_star_image(JObj &response, const json_value *params) {
    try {
        spdlog::debug("Try to get an image and transfer it to the client");
        int reqsize = 15;
        Params p("size", params);
        const json_value *val = p.param("size");
        if (val) {
            if (val->type != JSON_INT || (reqsize = val->int_value) < 15) {
                spdlog::error("Illegal image size parameter");
                response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                       "invalid image size param");
                return;
            }
        }
        VERIFY_GUIDER(response);
        Guider *guider = pFrame->pGuider;
        const usImage *img = guider->CurrentImage();
        const SodiumPoint &star = guider->CurrentPosition();
        if (guider->GetState() < GUIDER_STATE::STATE_SELECTED ||
            (img->ImageData == nullptr) || !star.IsValid()) {
            spdlog::error("No star selected!");
            response << jrpc_error(2, "no star selected");
            return;
        }
        int const halfw = wxMin((reqsize - 1) / 2, 31);
        int const fullw = 2 * halfw + 1;
        int const sx = (int)rint(star.X);
        int const sy = (int)rint(star.Y);
        wxRect rect(sx - halfw, sy - halfw, fullw, fullw);
        if (img->Subframe.IsEmpty()) {
            rect.Intersect(wxRect(img->Size));
        } else {
            rect.Intersect(img->Subframe);
        }
        B64Encode enc;
        for (int y = rect.GetTop(); y <= rect.GetBottom(); y++) {
            const unsigned short *p =
                img->ImageData +
                static_cast<ptrdiff_t>(y * img->Size.GetWidth()) +
                rect.GetLeft();
            enc.append(p, rect.GetWidth() * sizeof(unsigned short));
        }
        SodiumPoint pos(star);
        pos.X -= rect.GetLeft();
        pos.Y -= rect.GetTop();
        JObj rslt;
        rslt << NV("frame", img->FrameNum) << NV("width", rect.GetWidth())
             << NV("height", rect.GetHeight()) << NV("star_pos", pos)
             << NV("pixels", enc.finish());
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Exception caught in get_star_image: {}", e.what());
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, "internal error");
    }
}

static auto parse_settle(SettleParams *settle, const json_value *j,
                         wxString *error) -> bool {
    bool found_pixels = false;
    bool found_time = false;
    bool found_timeout = false;

    json_for_each(t, j) {
        if (float_param("pixels", t, &settle->tolerancePx)) {
            found_pixels = true;
            continue;
        }
        double d;
        if (float_param("time", t, &d)) {
            settle->settleTimeSec = (int)floor(d);
            found_time = true;
            continue;
        }
        if (float_param("timeout", t, &d)) {
            settle->timeoutSec = (int)floor(d);
            found_timeout = true;
            continue;
        }
    }

    settle->frames = 99999;

    bool ok = found_pixels && found_time && found_timeout;
    if (!ok) {
        *error = "invalid settle params";
    }

    return ok;
}

/// @brief start guiding
/// @param response JSON response object
/// @param params JSON request parameters
static void guide(JObj &response, const json_value *params) {
    try {
        SettleParams settle;
        // Parse settle parameters
        Params p("settle", "recalibrate", "roi", params);
        const json_value *p0 = p.param("settle");
        if ((p0 == nullptr) || p0->type != JSON_OBJECT) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected settle object param");
            return;
        }
        wxString errMsg;
        if (!parse_settle(&settle, p0, &errMsg)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS, errMsg);
            return;
        }
        // Parse recalibrate parameter
        bool recalibrate = false;
        const json_value *p1 = p.param("recalibrate");
        if ((p1 != nullptr) && !bool_param(p1, &recalibrate)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected bool value for recalibrate");
            return;
        }
        // Parse roi parameter
        wxRect roi;
        const json_value *p2 = p.param("roi");
        if ((p2 != nullptr) && !parse_rect(&roi, p2)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS, "invalid ROI param");
            return;
        }
        // Check if recalibration is allowed
        if (recalibrate && !pConfig->Global.GetBoolean(
                               "/server/guide_allow_recalibrate", true)) {
            Debug.AddLine(
                "ignoring client recalibration request since "
                "guide_allow_recalibrate = false");
            recalibrate = false;
        }
        // Set control options
        unsigned int ctrlOptions = GUIDEOPT_USE_STICKY_LOCK;
        if (recalibrate) {
            ctrlOptions |= GUIDEOPT_FORCE_RECAL;
        }
        // Check if guiding is possible
        wxString err;
        if (!PhdController::CanGuide(&err)) {
            response << jrpc_error(1, err);
            return;
        }
        // Start guiding
        if (PhdController::Guide(ctrlOptions, settle, roi, &err)) {
            response << jrpc_result(0);
        } else {
            response << jrpc_error(1, err);
        }
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief dither
/// @param response JSON response object
/// @param params JSON request parameters
static void dither(JObj &response, const json_value *params) {
    try {
        // Parse request parameters
        Params p("amount", "raOnly", "settle", params);
        const json_value *jv;
        double ditherAmt;
        jv = p.param("amount");
        if ((jv == nullptr) || !float_param(jv, &ditherAmt)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected dither amount param");
            return;
        }
        bool raOnly = false;
        jv = p.param("raOnly");
        if ((jv != nullptr) && !bool_param(jv, &raOnly)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected dither raOnly param");
            return;
        }
        SettleParams settle;
        jv = p.param("settle");
        if ((jv == nullptr) || jv->type != JSON_OBJECT) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected settle object param");
            return;
        }
        wxString errMsg;
        if (!parse_settle(&settle, jv, &errMsg)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS, errMsg);
            return;
        }
        // Check if dithering is possible
        wxString err;
        // Perform dithering
        if (PhdController::Dither(fabs(ditherAmt), raOnly, settle, &err)) {
            response << jrpc_result(0);
        } else {
            response << jrpc_error(1, err);
        }
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Shutdown Sodium via socket connection
/// @param response
/// @param params
static void shutdown(JObj &response,
                     [[maybe_unused]] const json_value *params) {
    spdlog::info("Shutdown Sodium by client , goodbye!");
    wxGetApp().TerminateApp();

    response << jrpc_result(0);
}

/// @brief Get current camera binning mode
/// @param response JSON response object
/// @param params JSON request parameters
static void get_camera_binning(JObj &response,
                               [[maybe_unused]] const json_value *params) {
    try {
        // Check if camera is connected
        if ((pCamera == nullptr) || !pCamera->Connected) {
            spdlog::error("Camera is not connected");
            response << jrpc_error(1, "camera not connected");
            return;
        }
        // Get current binning mode
        int binning = pCamera->Binning;
        spdlog::debug("Current camera binning mode is: {}", binning);
        response << jrpc_result(binning);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Get current camera frame size
/// @param response JSON response object
/// @param params JSON request parameters
static void get_camera_frame_size(JObj &response,
                                  [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Trying to get current camera frame size");
        // Check if camera is connected
        if ((pCamera == nullptr) || !pCamera->Connected) {
            spdlog::error("Camera is not connected");
            response << jrpc_error(1, "camera not connected");
            return;
        }
        // Get current frame size
        response << jrpc_result(pCamera->FullSize);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Check whether guiding output is enabled
/// @param response JSON response object
/// @param params JSON request parameters
static void get_guide_output_enabled(
    JObj &response, [[maybe_unused]] const json_value *params) {
    try {
        // Check if mount is defined
        if (pMount == nullptr) {
            response << jrpc_error(1, "mount not defined");
            return;
        }
        // Get guiding output status
        response << jrpc_result(pMount->GetGuidingEnabled());
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Set whether the guiding output is enabled
/// @param response JSON response object
/// @param params JSON request parameters
static void set_guide_output_enabled(JObj &response, const json_value *params) {
    try {
        // Parse request parameters
        Params p("enabled", params);
        const json_value *val = p.param("enabled");
        bool enable;
        if ((val == nullptr) || !bool_param(val, &enable)) {
            response << jrpc_error(JSONRPC_INVALID_PARAMS,
                                   "expected enabled boolean param");
            return;
        }
        // Set guiding output status
        if (pMount != nullptr) {
            pMount->SetGuidingEnabled(enable);
            response << jrpc_result(0);
        } else {
            response << jrpc_error(1, "mount not defined");
        }
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static auto axis_param(const Params &p, GuideAxis *a) -> bool {
    const json_value *val = p.param("axis");
    if ((val == nullptr) || val->type != JSON_STRING) {
        return false;
    }

    bool ok = true;

    if (wxStricmp(val->string_value, "ra") == 0) {
        *a = GUIDE_RA;
    } else if (wxStricmp(val->string_value, "x") == 0) {
        *a = GUIDE_X;
    } else if (wxStricmp(val->string_value, "dec") == 0) {
        *a = GUIDE_DEC;
    } else if (wxStricmp(val->string_value, "y") == 0) {
        *a = GUIDE_Y;
    } else {
        ok = false;
    }

    return ok;
}

/// @brief Get algorithm parameter names for a given axis
/// @param response JSON response object
/// @param params JSON request parameters
static void get_algo_param_names(JObj &response, const json_value *params) {
    try {
        Params p("axis", params);
        GuideAxis a;
        if (!axis_param(p, &a)) {
            spdlog::error("Expected axis name parameter");
            response << jrpc_error(1, "expected axis name param");
            return;
        }
        wxArrayString ary;
        ary.push_back("algorithmName");
        if (pMount != nullptr) {
            GuideAlgorithm *alg = a == GUIDE_X ? pMount->GetXGuideAlgorithm()
                                               : pMount->GetYGuideAlgorithm();
            alg->GetParamNames(ary);
        }
        JAry names;
        for (auto &it : ary) {
            names << ('"' + json_escape(it) + '"');
        }
        response << jrpc_result(names);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Get the value of a given algorithm parameter for a given axis
/// @param response JSON response object
/// @param params JSON request parameters
static void get_algo_param(JObj &response, const json_value *params) {
    try {
        Params p("axis", "name", params);
        GuideAxis a;
        if (!axis_param(p, &a)) {
            spdlog::error("Expected axis name parameter");
            response << jrpc_error(1, "expected axis name param");
            return;
        }
        const json_value *name = p.param("name");
        if ((name == nullptr) || name->type != JSON_STRING) {
            spdlog::error("Expected parameter name parameter");
            response << jrpc_error(1, "expected param name param");
            return;
        }
        bool ok = false;
        double val;
        if (pMount != nullptr) {
            GuideAlgorithm *alg = a == GUIDE_X ? pMount->GetXGuideAlgorithm()
                                               : pMount->GetYGuideAlgorithm();
            if (strcmp(name->string_value, "algorithmName") == 0) {
                response << jrpc_result(alg->GetGuideAlgorithmClassName());
                return;
            }
            ok = alg->GetParam(name->string_value, &val);
        }
        if (ok) {
            response << jrpc_result(val);
        } else {
            spdlog::error("Could not get parameter {}", name->string_value);
            response << jrpc_error(1, "could not get param");
        }
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static void set_algo_param(JObj &response, const json_value *params) {
    try {
        Params p("axis", "name", "value", params);
        GuideAxis a;
        if (!axis_param(p, &a)) {
            response << jrpc_error(1, "expected axis name param");
            return;
        }
        const json_value *name = p.param("name");
        if ((name == nullptr) || name->type != JSON_STRING) {
            response << jrpc_error(1, "expected param name param");
            return;
        }
        const json_value *val = p.param("value");
        double v;
        if (!float_param(val, &v)) {
            response << jrpc_error(1, "expected param value param");
            return;
        }
        bool ok = false;
        if (pMount != nullptr) {
            GuideAlgorithm *alg = a == GUIDE_X ? pMount->GetXGuideAlgorithm()
                                               : pMount->GetYGuideAlgorithm();
            ok = alg->SetParam(name->string_value, v);
        }
        if (ok) {
            response << jrpc_result(0);
            spdlog::info("Set algorithm parameter {} to {}", name->string_value,
                         v);
            if (pFrame->pGraphLog != nullptr) {
                pFrame->pGraphLog->UpdateControls();
            }
        } else {
            response << jrpc_error(1, "could not set param");
            spdlog::warn("Failed to set algorithm parameter {} to {}",
                         name->string_value, v);
        }
    } catch (std::exception &e) {
        response << jrpc_error(
            1, "exception caught while setting algorithm parameter");
        spdlog::error("Exception caught while setting algorithm parameter: {}",
                      e.what());
    } catch (...) {
        response << jrpc_error(
            1, "unknown exception caught while setting algorithm parameter");
        spdlog::error(
            "Unknown exception caught while setting algorithm parameter");
    }
}

/// @brief Get the current declination guide mode
/// @param response JSON response object
/// @param params JSON request parameters
static void get_dec_guide_mode(JObj &response,
                               [[maybe_unused]] const json_value *params) {
    try {
        Scope *scope = TheScope();
        DEC_GUIDE_MODE mode =
            (scope != nullptr) ? scope->GetDecGuideMode() : DEC_NONE;
        wxString s = Scope::DecGuideModeStr(mode);
        response << jrpc_result(s);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Set the declination guide mode
/// @param response JSON response object
/// @param params JSON request parameters
static void set_dec_guide_mode(JObj &response, const json_value *params) {
    try {
        Params p("mode", params);
        const json_value *mode = p.param("mode");
        if ((mode == nullptr) || mode->type != JSON_STRING) {
            spdlog::error("Expected mode parameter");
            response << jrpc_error(1, "expected mode param");
            return;
        }
        DEC_GUIDE_MODE m = DEC_AUTO;
        bool found = false;
        for (int im = DEC_NONE; im <= DEC_SOUTH; im++) {
            m = (DEC_GUIDE_MODE)im;
            if (wxStricmp(mode->string_value, Scope::DecGuideModeStr(m)) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            spdlog::error("Invalid dec guide mode parameter");
            response << jrpc_error(1, "invalid dec guide mode param");
            return;
        }
        Scope *scope = TheScope();
        if (scope != nullptr) {
            scope->SetDecGuideMode(m);
        }
        if (pFrame->pGraphLog != nullptr) {
            pFrame->pGraphLog->UpdateControls();
        }
        response << jrpc_result(0);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

/// @brief Get the settling status of the Sodium controller
/// @param response JSON response object
/// @param params JSON request parameters
static void get_settling(JObj &response,
                         [[maybe_unused]] const json_value *params) {
    try {
        bool settling = PhdController::IsSettling();
        response << jrpc_result(settling);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static GUIDE_DIRECTION dir_param(const json_value *p) {
    if ((p == nullptr) || p->type != JSON_STRING) {
        return GUIDE_DIRECTION::NONE;
    }

    struct ATOM_ALIGNAS(16) {
        const char *s;
        GUIDE_DIRECTION d;
    } dirs[] = {
        {"n", GUIDE_DIRECTION::NORTH},     {"s", GUIDE_DIRECTION::SOUTH},
        {"e", GUIDE_DIRECTION::EAST},      {"w", GUIDE_DIRECTION::WEST},
        {"north", GUIDE_DIRECTION::NORTH}, {"south", GUIDE_DIRECTION::SOUTH},
        {"east", GUIDE_DIRECTION::EAST},   {"west", GUIDE_DIRECTION::WEST},
        {"up", GUIDE_DIRECTION::UP},       {"down", GUIDE_DIRECTION::DOWN},
        {"left", GUIDE_DIRECTION::LEFT},   {"right", GUIDE_DIRECTION::RIGHT},
    };

    for (auto &dir : dirs) {
        if (wxStricmp(p->string_value, dir.s) == 0) {
            return dir.d;
        }
    }

    return GUIDE_DIRECTION::NONE;
}

static auto opposite(GUIDE_DIRECTION d) -> GUIDE_DIRECTION {
    switch (d) {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        case RIGHT:
            return LEFT;
        default:
            return d;
    }
}

/// @brief Issue a guide pulse to the mount or AO
/// @param response JSON response object
/// @param params JSON request parameters
static void guide_pulse(JObj &response, const json_value *params) {
    try {
        Params p("amount", "direction", "which", params);
        const json_value *amount = p.param("amount");
        if ((amount == nullptr) || amount->type != JSON_INT) {
            spdlog::error("Expected amount parameter");
            response << jrpc_error(1, "expected amount param");
            return;
        }
        GUIDE_DIRECTION dir = dir_param(p.param("direction"));
        if (dir == GUIDE_DIRECTION::NONE) {
            spdlog::error("Expected direction parameter");
            response << jrpc_error(1, "expected direction param");
            return;
        }
        WHICH_MOUNT which = which_mount(p.param("which"));
        Mount *m = nullptr;
        switch (which) {
            case MOUNT:
                m = TheScope();
                break;
            case AO:
                m = TheAO();
                break;
            case WHICH_MOUNT_BOTH:
            case WHICH_MOUNT_ERR:
                spdlog::error("Invalid 'which' parameter");
                response << jrpc_error(1, "invalid 'which' param");
                return;
        }
        if (!m || !m->IsConnected()) {
            spdlog::error("Device not connected");
            response << jrpc_error(1, "device not connected");
            return;
        }
        if (pFrame->pGuider->IsCalibratingOrGuiding() || m->IsBusy()) {
            spdlog::error(
                "Cannot issue guide pulse while calibrating or guiding");
            response << jrpc_error(
                1, "cannot issue guide pulse while calibrating or guiding");
            return;
        }
        int duration = amount->int_value;
        if (duration < 0) {
            duration = -duration;
            dir = opposite(dir);
        }
        pFrame->ScheduleManualMove(m, dir, duration);
        response << jrpc_result(0);
    } catch (std::exception &e) {
        response << jrpc_error(JSONRPC_INTERNAL_ERROR, e.what());
    }
}

static auto parity_str(GuideParity p) -> const char * {
    switch (p) {
        case GUIDE_PARITY_EVEN:
            return "+";
        case GUIDE_PARITY_ODD:
            return "-";
        default:
            return "?";
    }
}

static void get_calibration_data(JObj &response, const json_value *params) {
    Params p("which", params);                          // 解析输入参数
    WHICH_MOUNT which = which_mount(p.param("which"));  // 解析which参数
    Mount *m = nullptr;
    switch (which) {
        case MOUNT:
            m = TheScope();
            break;
        case AO:
            m = TheAO();
            break;
        case WHICH_MOUNT_BOTH:
        case WHICH_MOUNT_ERR: {
            response << jrpc_error(1, "invalid 'which' param");  // 发送错误响应
            return;
        }
    }
    if (!m || !m->IsConnected()) {  // 如果设备未连接，则发送错误响应
        response << jrpc_error(1, "device not connected");
        return;
    }
    JObj rslt;
    rslt << NV("calibrated", m->IsCalibrated());  // 添加已校准标志到响应结果
    if (m->IsCalibrated()) {  // 如果已校准，添加更多响应结果
        rslt << NV("xAngle", degrees(m->xAngle()), 1)
             << NV("xRate", m->xRate() * 1000.0, 3)
             << NV("xParity", parity_str(m->RAParity()))
             << NV("yAngle", degrees(m->yAngle()), 1)
             << NV("yRate", m->yRate() * 1000.0, 3)
             << NV("yParity", parity_str(m->DecParity()))
             << NV("declination", degrees(m->GetCalibrationDeclination()));
    }
    response << jrpc_result(rslt);  // 发送响应结果
}

static void get_cooler_status(JObj &response,
                              [[maybe_unused]] const json_value *params) {
    if ((pCamera == nullptr) ||
        !pCamera->Connected) {  // 如果相机未连接，则发送错误响应
        response << jrpc_error(1, "camera not connected");
        return;
    }
    bool on;
    double setpoint;
    double power;
    double temperature;
    bool err = pCamera->GetCoolerStatus(&on, &setpoint, &power,
                                        &temperature);  // 获取冷却器状态
    if (err) {  // 如果获取失败，则发送错误响应
        response << jrpc_error(1, "failed to get cooler status");
        return;
    }
    JObj rslt;
    rslt << NV("coolerOn", on) << NV("temperature", temperature, 1);
    if (on) {  // 如果冷却器打开，添加更多响应结果
        rslt << NV("setpoint", setpoint, 1) << NV("power", power, 1);
    }
    response << jrpc_result(rslt);  // 发送响应结果
}

/// @brief Get current tempeture of the camera
/// @param response
/// @param params {}
static void get_sensor_temperature(JObj &response,
                                   [[maybe_unused]] const json_value *params) {
    if ((pCamera == nullptr) ||
        !pCamera->Connected) {  // 如果相机未连接，则发送错误响应
        spdlog::error("Camera is not connected");
        response << jrpc_error(1, "camera not connected");
        return;
    }
    double temperature;
    bool err = pCamera->GetSensorTemperature(&temperature);  // 获取相机当前温度
    if (err) {  // 如果获取失败，则发送错误响应
        spdlog::error("Failed to get camera current temperature");
        response << jrpc_error(1, "failed to get sensor temperature");
        return;
    }
    spdlog::debug("Current temperature : {}", temperature);
    JObj rslt;
    rslt << NV("temperature", temperature, 1);  // 添加温度到响应结果
    response << jrpc_result(rslt);              // 发送响应结果
}

/// @brief export the settings to a local file
/// @param response
/// @param params {}
static void export_config_settings(JObj &response,
                                   [[maybe_unused]] const json_value *params) {
    spdlog::debug("Attempt to export configuration file to local");
    wxString filename(MyFrame::GetDefaultFileDir() + PATHSEPSTR +
                      "lightguider_settings.txt");
    bool err = pConfig->SaveAll(filename);  // 将配置保存到文件中
    if (err) {  // 如果保存失败，则发送错误响应
        spdlog::error("Faild to export settings");
        response << jrpc_error(1, "export settings failed");
        return;
    }
    JObj rslt;
    rslt << NV("filename", filename);  // 添加文件名到响应结果
    response << jrpc_result(rslt);     // 发送响应结果
}

struct JRpcCall {
    wxSocketClient *cli;
    const json_value *req;
    const json_value *method;
    JRpcResponse response;

    JRpcCall(wxSocketClient *cli_, const json_value *req_)
        : cli(cli_), req(req_), method(nullptr) {}
} ATOM_ALIGNAS(128);

static void dump_request(const JRpcCall &call) {
    Debug.Write(wxString::Format("evsrv: cli %p request: %s\n", call.cli,
                                 json_format(call.req)));
}

static void dump_response(const JRpcCall &call) {
    wxString s(const_cast<JRpcResponse &>(call.response).str());

    // trim output for huge responses

    // this is very hacky operating directly on the string, but it's not
    // worth bothering to parse and reformat the response
    if ((call.method != nullptr) &&
        strcmp(call.method->string_value, "get_star_image") == 0) {
        size_t p0;
        size_t p1;
        if ((p0 = s.find(R"("pixels":")")) != wxString::npos &&
            (p1 = s.find('"', p0 + 10)) != wxString::npos)
            s.replace(p0 + 10, p1 - (p0 + 10), "...");
    }

    Debug.Write(wxString::Format("evsrv: cli %p response: %s\n", call.cli, s));
}

/// @brief Just a response to check whether the server is modified by LightAPT
/// @param response
/// @param params
static void lightapt_modify_response(
    JObj &response, [[maybe_unused]] const json_value *params) {
    try {
        spdlog::info(
            "Verification succeeded. The client is a LightAPT client.");
        JObj rslt;
        rslt << NV("version", "2.6.11-patch1");
        rslt << NV("modified", true);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Exception occurred in lightapt_modify_response(): {}",
                      e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "An error occurred while processing your request.");
        response << jrpc_error(-1, "Internal server error");
    }
}

// The following codes are written by Max Qian for LightAPT.
/// @brief Check if the dark images library is loaded.
/// @param response
/// @param params
static void is_darklib_loaded(JObj &response,
                              [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Check whether the dark-field library has been loaded.");
        bool loaded = pFrame->LoadDarkLibrary();
        if (loaded) {
            spdlog::debug("Dark-field library has been loaded.");
        } else {
            spdlog::debug("Dark-field library was not loaded.");
        }
        JObj rslt;
        rslt << NV("status", true);
        rslt << NV("loaded", loaded);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Exception occurred in is_darklib_loaded(): {}",
                      e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "An error occurred while processing your request.");
        response << jrpc_error(-1, "Internal server error");
    }
}

/// @brief Get the full path to the dark library.
/// @param response
/// @param params
static void get_darklib_path(JObj &response,
                             [[maybe_unused]] const json_value *params) {
    try {
        spdlog::debug("Try to get the dark field library path.");
        wxString path = MyFrame::GetDarksDir();
        spdlog::debug("DarkLib path: {}", std::string(path));
        JObj rslt;
        rslt << NV("status", true);
        rslt << NV("path", path);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Exception occurred in get_darklib_path(): {}", e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "An error occurred while processing your request.");
        response << jrpc_error(-1, "Internal server error");
    }
}

/// @brief Get the name of the dark library.
/// @param response
/// @param params
static void get_darklib_name(JObj &response, const json_value *params) {
    try {
        spdlog::debug("Try to get the name of darkfield library by number.");
        Params p("profile_id", params);
        const json_value *profile_id = p.param("profile_id");
        JObj rslt;
        if (profile_id == nullptr) {
            spdlog::error("The profile id must be provided.");
            rslt << NV("status", false);
            rslt << NV("error", "The file id must be provided.");
            response << jrpc_result(rslt);
            return;
        }
        wxString name = MyFrame::DarkLibFileName(profile_id->int_value);
        spdlog::debug("Dark lib name: {}, profile id: {}", std::string(name),
                      profile_id->int_value);
        rslt << NV("status", true);
        rslt << NV("name", name);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Exception occurred in get_darklib_name(): {}", e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "An error occurred while processing your request.");
        response << jrpc_error(-1, "Internal server error");
    }
}

/// @brief Set which dark library to be used as the main dark frames.
/// @param response
/// @param params
static void set_darklib(JObj &response, const json_value *params) {
    try {
        spdlog::debug("Try to set the current darkfield library.");
        Params p("library_path", params);
        const json_value *library_path = p.param("library_path");
        JObj rslt;
        if (library_path == nullptr) {
            spdlog::error("The library path must be provided.");
            rslt << NV("status", false);
            rslt << NV("error", "The library path must be provided.");
            response << jrpc_result(rslt);
            return;
        }
        wxString path = library_path->string_value;
        bool success = true;
        if (success) {
            spdlog::info(
                "Successfully set the current darkfield library to: {}",
                std::string(path));
            rslt << NV("status", true);
            rslt << NV("message",
                       "Successfully set the current darkfield library.");
            response << jrpc_result(rslt);
        } else {
            spdlog::error("Failed to set the current darkfield library to: {}",
                          std::string(path));
            rslt << NV("status", false);
            rslt << NV("error", "Failed to set the current darkfield library.");
            response << jrpc_result(rslt);
        }
    } catch (const std::exception &e) {
        spdlog::error("Exception occurred in set_darklib(): {}", e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "An error occurred while processing your request.");
        response << jrpc_error(-1, "Internal server error");
    }
}

/// @brief Start Dark Library Image Capture
/// @param response JRPC response object
/// @param params JRPC parameters object
static void start_darklib_capture(JObj &response, const json_value *params) {
    try {
        // Check if the guider is calibrating or guiding
        if (pFrame->pGuider->IsCalibratingOrGuiding() ||
            pFrame->pGuider->IsGuiding()) {
            response << jrpc_error(
                1, "Cannot issue guide pulse while calibrating or guiding");
            spdlog::error(
                "Cannot issue guide pulse while calibrating or guiding");
            return;
        }
        spdlog::debug("Try to build a dark field library");
        // Check whether the camera has a shutter, if not, warn the user to
        // cover the scope This should be finished at the client side, there
        // will not return any message
        if (!pCamera->HasShutter) {
            // wxMessageBox(_("Cover guide scope"));
        }
        // Set a flag to indicate that the shutter is closed
        pCamera->ShutterClosed = true;
        bool err = false;
        // Load the required parameters
        // name : the name of the dark library to save
        // max_exposure : the max value of the exposure
        // min_exposure : the min value of the exposure
        // count : the count of the image each exposure time to capture
        // tip : additional notes or tips
        Params p("name", "max_exposure", "min_exposure", "count", "tip",
                 params);
        const json_value *name = p.param("name");
        const json_value *max_exposure = p.param("max_exposure");
        const json_value *min_exposure = p.param("min_exposure");
        const json_value *count = p.param("count");
        const json_value *tip = p.param("tip");
        JObj rslt;
        // Check if the required parameters are provided
        if (max_exposure == nullptr) {
            spdlog::error("Max exposure time should be given");
            rslt << NV("error", "Max exposure time should be given");
            response << jrpc_result(rslt);
            return;
        }
        if (min_exposure == nullptr) {
            spdlog::error("Min exposure time should be given");
            rslt << NV("error", "Min exposure time should be given");
            response << jrpc_result(rslt);
            return;
        }
        if (count == nullptr) {
            spdlog::error("Number of the dark frames should be given");
            rslt << NV("error", "Number of the dark frames should be given");
            response << jrpc_result(rslt);
            return;
        }
        // Check if the camera is connected
        if ((pCamera == nullptr) || !pCamera->Connected) {
            spdlog::error("Camera is not connected");
            rslt << NV("error", err);
            rslt << NV("message", "Camera is not connected");
            response << jrpc_result(rslt);
            return;
        }
        DarksDialog dlg(pFrame, true);
        spdlog::debug("Try to set dark field library parameters");
        wxString max;
        wxString min;
        // Set the parameters in the dialog box
        dlg.m_pDarkMinExpTime->SetValue(
            wxString::Format("%f s", min_exposure->float_value));
        dlg.m_pDarkMaxExpTime->SetValue(
            wxString::Format("%f s", max_exposure->float_value));
        dlg.m_pDarkCount->SetValue(count->int_value);
        if (tip != nullptr) {
            dlg.m_pNotes->SetValue(wxString(tip->string_value));
        }
        // Start building the dark image library
        err = dlg.OnServerStart();
        if (err) {
            spdlog::error("Failed to create dark image library");
            rslt << NV("status", err);
            rslt << NV("message", "Failed to create dark image library");
        } else {
            // Put up a message showing results and maybe notice to uncover the
            // scope; then close the dialog
            pCamera->ShutterClosed = false;  // Lights
            // if (!pCamera->HasShutter)
            // wrapupMsg = _("Uncover guide scope") + wxT("\n\n") + wrapupMsg;
            // // Results will appear in smaller font
            // wxMessageBox(wxString::Format(_("Operation complete: %s"),
            // wrapupMsg));
            spdlog::debug("Created dark lib successfully. Operation complete.");
        }
        response << jrpc_result(rslt);
    } catch (const std::exception &e)  // Catch any exception and log it
    {
        spdlog::error("Exception in start_darklib_capture: {}", e.what());
        response << jrpc_error(1, "Exception in start_darklib_capture");
    }
}

/**
 * @brief Stop Dark Library Image Capture
 * @param response
 * @param params
 */
static void stop_darklib_capture(JObj &response,
                                 [[maybe_unused]] const json_value *params) {
    try {
        // 添加调试输出
        spdlog::debug("Try to stop the dark-field library shooting");
    } catch (const std::exception &e) {
        // 添加异常捕获和处理
        spdlog::error("Failed to stop the dark-field library shooting: {}",
                      e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "Failed to stop the dark-field library shooting");
        response << jrpc_result(rslt);
        return;
    }
}
/**
 * @brief Delete Dark Library by Profile ID
 * @param response
 * @param params
 */
static void delete_darklib_by_profile(JObj &response,
                                      const json_value *params) {
    try {
        Params p("id", params);
        const json_value *profile_id = p.param("id");
        JObj rslt;
        if (profile_id == nullptr) {
            spdlog::error("No profile id was given");
            rslt << NV("status", false);
            rslt << NV("error", "A profile id must be given");
            response << jrpc_result(rslt);
            return;
        }
        MyFrame::DeleteDarkLibraryFiles(profile_id->int_value);
        // 添加调试输出
        spdlog::debug("Deleted the dark lib by profile id successfully");
        rslt << NV("status", true);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        // 添加异常捕获和处理
        spdlog::error("Failed to delete the dark lib by profile id: {}",
                      e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "Failed to delete the dark lib by profile id");
        response << jrpc_result(rslt);
    }
}

/**
 * @brief Create a new profile
 * @param response
 * @param params
 */
static void create_profile(JObj &response, const json_value *params) {
    try {
        // 添加调试输出
        spdlog::debug("Try to create a new profile");
        Params p("name", params);
        const json_value *name = p.param("name");
        JObj rslt;
        if (name == nullptr) {
            spdlog::error(
                "Must provide the name of the file you want to create");
            rslt << NV("status", false);
            rslt << NV("error",
                       " the name of the file want to create must be provide");
            response << jrpc_result(rslt);
            return;
        }
        bool status = pConfig->CreateProfile(name->string_value);
        pConfig->InitializeProfile();
        rslt << NV("status", status);
        if (status) {
            // 添加调试输出
            spdlog::debug("Created a new profile successfully");
        } else {
            spdlog::error("Failed to create a new profile");
            rslt << NV("error", "Failed to create a new profile");
        }
        response << jrpc_result(rslt);
        // unsigned int number =  pConfig -> NumProfiles();
        // Profile.m_pConfig->Write(wxString::Format("/profile/%d/name", id),
        // number+1);
        EvtServer.NotifyConfigurationChange();
    } catch (const std::exception &e) {
        // 添加异常捕获和处理
        spdlog::error("Failed to create a new profile: {}", e.what());
        JObj rslt;
        rslt << NV("status", false);
        rslt << NV("error", "Failed to create a new profile");
        response << jrpc_result(rslt);
    }
}

auto isStringInList(const std::string &targetStr,
                    const std::vector<std::string> &strList) -> bool {
    auto it = std::find(strList.begin(), strList.end(), targetStr);
    return it != strList.end();
}

using hash_t = std::uint64_t;
using StringList = std::vector<std::string>;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

/*以下三个函数均是用于switch支持string*/
constexpr auto hashCompileTime(char const *str,
                               hash_t last_value = basis) -> hash_t {
    return (*str != 0) ? hashCompileTime(str + 1, (*str ^ last_value) * prime)
                       : last_value;
}

auto hash_(char const *str) -> hash_t {
    hash_t ret{basis};
    while (*str) {
        ret ^= *str;
        ret *= prime;
        str++;
    }
    return ret;
}

constexpr auto operator"" _hash(char const *p, size_t) -> unsigned long long {
    return hashCompileTime(p);
}

/// @brief Update the infomation in the profile
/// @param response 用于返回JRPC响应结果
/// @param params 包含type, name, value三个参数的json对象
static void update_profile(JObj &response, const json_value *params) {
    spdlog::debug("Trying to update the profile with given parameters");
    Params p("type", "name", "value", params);
    const json_value *type = p.param("type");
    const json_value *name = p.param("name");
    const json_value *value = p.param("value");
    JObj rslt;
    // 检查参数是否合法
    if (!type) {
        spdlog::error("the type of the parameter must be provide");
        rslt << NV("status", false);
        rslt << NV("error", " the type of the parameter must be provide");
        response << jrpc_result(rslt);
        return;
    }
    if (!name) {
        spdlog::error("the name of the parameter must be provide");
        rslt << NV("status", false);
        rslt << NV("error", " the name of the parameter must be provide");
        response << jrpc_result(rslt);
        return;
    }
    if (!value) {
        spdlog::error("the value of the parameter must be provide");
        rslt << NV("status", false);
        rslt << NV("error", " the value of the parameter must be provide");
        response << jrpc_result(rslt);
        return;
    }
    // 根据type的值进行不同的更新操作
    switch (hash_(type->string_value)) {
        case "auto_exp"_hash: {
            // TODO: 在这里添加自动计算经验的更新操作
            break;
        }
        case "CalStepCalc"_hash: {
            // TODO: 在这里添加计算步进的更新操作
            break;
        }
        default:
            break;
    }

    // 将参数写入配置文件
    wxString command;
    pConfig->Profile.SetInt(
        command.Format("/%s/%s", type->string_value, name->string_value),
        value->int_value);
}

/// @brief Rename a profile to a new name
/// @param response 用于返回JRPC响应结果
/// @param params 包含oldname和newname两个参数的json对象
static void rename_profile(JObj &response, const json_value *params) {
    Params p("oldname", "newname", params);
    const json_value *oldname = p.param("oldname");
    const json_value *newname = p.param("newname");
    JObj rslt;
    // 检查参数是否合法
    if (!oldname || !newname) {
        spdlog::debug(
            "The original name and new name of the file to be modified must be "
            "provided");
        rslt << NV("status", false);
        rslt << NV("error",
                   "The original name and new name of the file to be modified "
                   "must be provided");
        response << jrpc_result(rslt);
        return;
    }
    // 重命名配置文件
    bool status =
        pConfig->RenameProfile(oldname->string_value, newname->string_value);

    rslt << NV("status", status);
    if (!status) {
        spdlog::error("Failed to rename the profile from {} to {}",
                      oldname->string_value, newname->string_value);
        rslt << NV("error", "Failed to rename the profile");
    } else {
        spdlog::debug("Renamed a profile successfully ...");
    }
    response << jrpc_result(rslt);
}

/// @brief Copy a profile to new destination
/// @param response 用于返回JRPC响应结果
/// @param params 包含dest和source两个参数的json对象
static void copy_profile(JObj &response, const json_value *params) {
    Params p("dest", "source", params);
    const json_value *source = p.param("source");
    const json_value *dest = p.param("dest");
    JObj rslt;
    // 检查参数是否合法
    if (!source || !dest) {
        spdlog::error(
            "The source file and address of the replication profile must be "
            "provided");
        rslt << NV("status", false);
        rslt << NV("error",
                   "The source file and address of the replication profile "
                   "must be provided");
        response << jrpc_result(rslt);
        return;
    }
    spdlog::debug("Trying to copy the profile from {} to {}",
                  source->string_value, dest->string_value);

    // 复制配置文件
    bool status =
        pConfig->CloneProfile(wxString(source->string_value, wxConvUTF8),
                              wxString(dest->string_value, wxConvUTF8));

    if (!status) {
        spdlog::error("Failed to copy the profile");
        rslt << NV("status", status);
        rslt << NV("error", "Failed to copy the profile");
    } else {
        spdlog::debug("Copy the profile successfully");
        rslt << NV("status", status);
    }
    response << jrpc_result(rslt);
}

/// @brief Delete the profile by name
/// @param response 用于返回JRPC响应结果
/// @param params 包含name参数的json对象
static void delete_profile(JObj &response, const json_value *params) {
    Params p("name", params);
    const json_value *name = p.param("name");
    JObj rslt;
    // 检查参数是否合法
    if (!name) {
        spdlog::error("The profile ID to be deleted must be provided");
        rslt << NV("status", false);
        rslt << NV("error", "The profile ID to be deleted must be provided");
        response << jrpc_result(rslt);
        return;
    }
    spdlog::debug("Trying to delete {} profile", name->string_value);
    try {
        // 删除配置文件
        pConfig->DeleteProfile(name->string_value);
        spdlog::debug("Deleted the {} profile", name->string_value);
        rslt << NV("status", true);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Error occurred while deleting profile: {}", e.what());
        rslt << NV("status", false);
        rslt << NV("error", e.what());
        response << jrpc_result(rslt);
    }
}

/// @brief Delete all of the profiles and do not create a new one automatically
/// @param response 用于返回JRPC响应结果
/// @param params
static void delete_all_profiles(JObj &response,
                                [[maybe_unused]] const json_value *params) {
    JObj rslt;
    try {
        // 删除所有配置文件
        pConfig->DeleteAll();
        spdlog::debug("Deleted all of the profiles Sodium found");
        rslt << NV("status", true);
        response << jrpc_result(rslt);
    } catch (const std::exception &e) {
        spdlog::error("Error occurred while deleting all profiles: {}",
                      e.what());
        rslt << NV("status", false);
        rslt << NV("error", e.what());
        response << jrpc_result(rslt);
    }
}

static bool handle_request(JRpcCall &call) {
    const json_value *params;
    const json_value *id;

    dump_request(call);

    parse_request(call.req, &call.method, &params, &id);

    if (!call.method) {
        call.response << jrpc_error(JSONRPC_INVALID_REQUEST,
                                    "invalid request - missing method")
                      << jrpc_id(0);
        return true;
    }

    if (params && params->type != JSON_ARRAY && params->type != JSON_OBJECT) {
        call.response
            << jrpc_error(JSONRPC_INVALID_REQUEST,
                          "invalid request - params must be an array or object")
            << jrpc_id(0);
        return true;
    }

    static struct {
        const char *name;
        void (*fn)(JObj &response, const json_value *params);
    } methods[] = {{
                       "clear_calibration",
                       &clear_calibration,
                   },
                   {
                       "deselect_star",
                       &deselect_star,
                   },
                   {
                       "get_exposure",
                       &get_exposure,
                   },
                   {
                       "set_exposure",
                       &set_exposure,
                   },
                   {
                       "get_exposure_durations",
                       &get_exposure_durations,
                   },
                   {
                       "get_profiles",
                       &get_profiles,
                   },
                   {
                       "get_profile",
                       &get_profile,
                   },
                   {
                       "set_profile",
                       &set_profile,
                   },
                   {
                       "get_connected",
                       &get_connected,
                   },
                   {
                       "set_connected",
                       &set_connected,
                   },
                   {
                       "get_calibrated",
                       &get_calibrated,
                   },
                   {
                       "get_paused",
                       &get_paused,
                   },
                   {
                       "set_paused",
                       &set_paused,
                   },
                   {
                       "get_lock_position",
                       &get_lock_position,
                   },
                   {
                       "set_lock_position",
                       &set_lock_position,
                   },
                   {
                       "loop",
                       &loop,
                   },
                   {
                       "stop_capture",
                       &stop_capture,
                   },
                   {
                       "guide",
                       &guide,
                   },
                   {
                       "dither",
                       &dither,
                   },
                   {
                       "find_star",
                       &find_star,
                   },
                   {
                       "get_pixel_scale",
                       &get_pixel_scale,
                   },
                   {
                       "get_app_state",
                       &get_app_state,
                   },
                   {
                       "flip_calibration",
                       &flip_calibration,
                   },
                   {
                       "get_lock_shift_enabled",
                       &get_lock_shift_enabled,
                   },
                   {
                       "set_lock_shift_enabled",
                       &set_lock_shift_enabled,
                   },
                   {
                       "get_lock_shift_params",
                       &get_lock_shift_params,
                   },
                   {
                       "set_lock_shift_params",
                       &set_lock_shift_params,
                   },
                   {
                       "save_image",
                       &save_image,
                   },
                   {
                       "get_star_image",
                       &get_star_image,
                   },
                   {
                       "get_use_subframes",
                       &get_use_subframes,
                   },
                   {
                       "get_search_region",
                       &get_search_region,
                   },
                   {
                       "shutdown",
                       &shutdown,
                   },
                   {
                       "get_camera_binning",
                       &get_camera_binning,
                   },
                   {
                       "get_camera_frame_size",
                       &get_camera_frame_size,
                   },
                   {
                       "get_current_equipment",
                       &get_current_equipment,
                   },
                   {
                       "get_guide_output_enabled",
                       &get_guide_output_enabled,
                   },
                   {
                       "set_guide_output_enabled",
                       &set_guide_output_enabled,
                   },
                   {
                       "get_algo_param_names",
                       &get_algo_param_names,
                   },
                   {
                       "get_algo_param",
                       &get_algo_param,
                   },
                   {
                       "set_algo_param",
                       &set_algo_param,
                   },
                   {
                       "get_dec_guide_mode",
                       &get_dec_guide_mode,
                   },
                   {
                       "set_dec_guide_mode",
                       &set_dec_guide_mode,
                   },
                   {
                       "get_settling",
                       &get_settling,
                   },
                   {
                       "guide_pulse",
                       &guide_pulse,
                   },
                   {
                       "get_calibration_data",
                       &get_calibration_data,
                   },
                   {
                       "capture_single_frame",
                       &capture_single_frame,
                   },
                   {
                       "get_cooler_status",
                       &get_cooler_status,
                   },
                   {
                       "get_ccd_temperature",
                       &get_sensor_temperature,
                   },
                   {
                       "export_config_settings",
                       &export_config_settings,
                   },

                   {"lightapt_modify_response", &lightapt_modify_response},

                   {"get_darklib_path", &get_darklib_path},
                   {"set_darklib", &set_darklib},
                   {"get_darklib_name", &get_darklib_name},
                   {"is_darklib_loaded", &is_darklib_loaded},
                   {"create_darklib", &start_darklib_capture},
                   {"stop_darklib_capture", &stop_darklib_capture},

                   {"create_profile", &create_profile},
                   {"rename_profile", &rename_profile},
                   {"copy_profile", &copy_profile},
                   {"update_profile", &update_profile},
                   {"delete_profile", &delete_profile},
                   {"delete_all_profiles", &delete_all_profiles}};

    for (auto &method : methods) {
        if (strcmp(call.method->string_value, method.name) == 0) {
            (*method.fn)(call.response, params);
            if (id != nullptr) {
                call.response << jrpc_id(id);
                return true;
            }
            return false;
        }
    }

    if (id != nullptr) {
        call.response << jrpc_error(JSONRPC_METHOD_NOT_FOUND,
                                    "method not found")
                      << jrpc_id(id);
        return true;
    }
    return false;
}

static void handle_cli_input_complete(wxSocketClient *cli, char *input,
                                      JsonParser &parser) {
    if (!parser.Parse(input)) {
        JRpcCall call(cli, nullptr);
        call.response << jrpc_error(JSONRPC_PARSE_ERROR, parser_error(parser))
                      << jrpc_id(0);
        dump_response(call);
        do_notify1(cli, call.response);
        return;
    }

    const json_value *root = parser.Root();

    if (root->type == JSON_ARRAY) {
        // a batch request

        JAry ary;

        bool found = false;
        json_for_each(req, root) {
            JRpcCall call(cli, req);
            if (handle_request(call)) {
                dump_response(call);
                ary << call.response;
                found = true;
            }
        }

        if (found)
            do_notify1(cli, ary);
    } else {
        // a single request

        const json_value *const req = root;
        JRpcCall call(cli, req);
        if (handle_request(call)) {
            dump_response(call);
            do_notify1(cli, call.response);
        }
    }
}

static void handle_cli_input(wxSocketClient *cli, JsonParser &parser) {
    // Bump refcnt to protect against reentrancy.
    //
    // Some functions like set_connected can cause the event loop to run
    // reentrantly. If the client disconnects before the response is sent and a
    // socket disconnect event is dispatched the client data could be destroyed
    // before we respond.

    ClientDataGuard clidata(cli);

    ClientReadBuf *rdbuf = &clidata->rdbuf;

    wxSocketInputStream sis(*cli);

    while (sis.CanRead()) {
        if (rdbuf->avail() == 0) {
            drain_input(sis);

            JRpcResponse response;
            response << jrpc_error(JSONRPC_INTERNAL_ERROR, "too big")
                     << jrpc_id(0);
            do_notify1(cli, response);

            rdbuf->reset();
            break;
        }
        size_t n = sis.Read(rdbuf->dest, rdbuf->avail()).LastRead();
        if (n == 0)
            break;

        rdbuf->dest += n;

        char *end;
        while ((end = static_cast<char *>(
                    memchr(rdbuf->buf(), '\n', rdbuf->len()))) != nullptr) {
            // Copy to temporary buffer to avoid rentrancy problem
            char line[ClientReadBuf::SIZE];
            size_t len1 = end - rdbuf->buf();
            memcpy(line, rdbuf->buf(), len1);
            line[len1] = 0;

            char *next = end + 1;
            size_t len2 = rdbuf->dest - next;
            memmove(rdbuf->buf(), next, len2);
            rdbuf->dest = rdbuf->buf() + len2;

            handle_cli_input_complete(cli, line, parser);
        }
    }
}

EventServer::EventServer() : m_configEventDebouncer(nullptr) {}

EventServer::~EventServer() {}

/// @brief Start the socket server non-blocking
/// @param instanceId The unique identifier for the instance of the server being
/// started
/// @return bool Returns true if the server was started successfully, false
/// otherwise
bool EventServer::EventServerStart(unsigned int instanceId) {
    // Check if the server is already running
    if (m_serverSocket) {
        spdlog::warn(
            "Attempt to start event server when it is already started?");
        Debug.AddLine(
            "attempt to start event server when it is already started?");
        return false;
    }
    // Calculate the port number based on the instance ID
    unsigned int port = 4400 + instanceId - 1;
    // Create a new IPV4 address for the event server
    wxIPV4address eventServerAddr;
    eventServerAddr.Service(port);
    // Create a new socket server
    m_serverSocket = new wxSocketServer(eventServerAddr, wxSOCKET_REUSEADDR);
    // Check if the socket server was created successfully
    if (!m_serverSocket->Ok()) {
        spdlog::error("Failed to start server at port {}", port);
        Debug.Write(wxString::Format(
            "Event server failed to start - Could not listen at port %u\n",
            port));
        delete m_serverSocket;
        m_serverSocket = nullptr;
        return true;
    }
    // Set the event handler for the socket server and enable notifications for
    // new connections
    m_serverSocket->SetEventHandler(*this, EVENT_SERVER_ID);
    m_serverSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_serverSocket->Notify(true);
    // Create a new timer for the server's configuration event debouncer
    m_configEventDebouncer = new wxTimer();
    spdlog::debug("Started socket server successfully , listening on port {}",
                  port);
    Debug.Write(
        wxString::Format("event server started, listening on port %u\n", port));
    // Return success
    return true;
}

/// @brief Stop the socket server and all connected clients
void EventServer::EventServerStop() {
    spdlog::debug("Trying to stop the socket event server");
    // Check if the server is running
    if (!m_serverSocket) {
        spdlog::error("There is not server running");
        return;
    }
    // Disconnect all connected clients
    for (CliSockSet::const_iterator it = m_eventServerClients.begin();
         it != m_eventServerClients.end(); ++it) {
        destroy_client(*it);
    }
    m_eventServerClients.clear();
    // Delete the server socket and configuration event debouncer
    delete m_serverSocket;
    m_serverSocket = nullptr;
    delete m_configEventDebouncer;
    m_configEventDebouncer = nullptr;
    spdlog::debug("Stopped socket server successfully");
    Debug.AddLine("event server stopped");
}

/// @brief Handle events for the socket server
void EventServer::OnEventServerEvent(wxSocketEvent &event) {
    // Get the server socket
    wxSocketServer *server = static_cast<wxSocketServer *>(event.GetSocket());
    // If the event is not a new connection, return
    if (event.GetSocketEvent() != wxSOCKET_CONNECTION)
        return;
    // Accept the new client connection
    wxSocketClient *client =
        static_cast<wxSocketClient *>(server->Accept(false));
    // If the client could not be connected, return
    if (!client)
        return;
    Debug.Write(wxString::Format("evsrv: cli %p connect\n", client));
    // Set the event handler for the client and enable notifications for lost
    // connections and input data
    client->SetEventHandler(*this, EVENT_SERVER_CLIENT_ID);
    client->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
    client->SetFlags(wxSOCKET_NOWAIT);
    client->Notify(true);

    // Set client data to a new instance of ClientData
    client->SetClientData(new ClientData(client));
    // Send any catch-up events to the newly connected client
    send_catchup_events(client);
    // Add the new client to the set of connected clients
    m_eventServerClients.insert(client);
}
/// @brief Handle events for a connected client
void EventServer::OnEventServerClientEvent(wxSocketEvent &event) {
    // Get the client socket
    wxSocketClient *cli = static_cast<wxSocketClient *>(event.GetSocket());
    // If the event is a lost connection, remove the client and destroy it
    if (event.GetSocketEvent() == wxSOCKET_LOST) {
        Debug.Write(wxString::Format("evsrv: cli %p disconnect\n", cli));
        unsigned int const n = m_eventServerClients.erase(cli);
        if (n != 1)
            Debug.AddLine("client disconnected but not present in client set!");
        destroy_client(cli);
    }
    // If the event is input data, handle it using the parser
    else if (event.GetSocketEvent() == wxSOCKET_INPUT) {
        handle_cli_input(cli, m_parser);
    }
    // Otherwise, log an unexpected event
    else {
        Debug.Write(wxString::Format("unexpected client socket event %d\n",
                                     event.GetSocketEvent()));
    }
}

void EventServer::NotifyStartCalibration(const Mount *mount) {
    SIMPLE_NOTIFY_EV(ev_start_calibration(mount));
}

void EventServer::NotifyCalibrationStep(const CalibrationStepInfo &info) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("Calibrating");

    ev << NVMount(info.mount) << NV("dir", info.direction)
       << NV("dist", info.dist) << NV("dx", info.dx) << NV("dy", info.dy)
       << NV("pos", info.pos) << NV("step", info.stepNumber);

    if (!info.msg.empty())
        ev << NV("State", info.msg);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyCalibrationFailed(const Mount *mount,
                                          const wxString &msg) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("CalibrationFailed");
    ev << NVMount(mount) << NV("Reason", msg);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyCalibrationComplete(const Mount *mount) {
    if (m_eventServerClients.empty())
        return;

    do_notify(m_eventServerClients, ev_calibration_complete(mount));
}

void EventServer::NotifyCalibrationDataFlipped(const Mount *mount) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("CalibrationDataFlipped");
    ev << NVMount(mount);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyLooping(unsigned int exposure, const Star *star,
                                const FrameDroppedInfo *info) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("LoopingExposures");
    ev << NV("Frame", exposure);

    double mass = 0., snr, hfd;
    int err = 0;
    wxString status;

    if (star) {
        mass = star->Mass;
        snr = star->SNR;
        hfd = star->HFD;
        err = star->GetError();
    } else if (info) {
        if (Star::WasFound(static_cast<Star::FindResult>(info->starError))) {
            mass = info->starMass;
            snr = info->starSNR;
            hfd = info->starHFD;
        }
        err = info->starError;
        status = info->status;
    }

    if (mass) {
        ev << NV("StarMass", mass, 0) << NV("SNR", snr, 2) << NV("HFD", hfd, 2);
    }

    if (err)
        ev << NV("ErrorCode", err);

    if (!status.IsEmpty())
        ev << NV("Status", status);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyLoopingStopped() {
    SIMPLE_NOTIFY("LoopingExposuresStopped");
}

void EventServer::NotifyStarSelected(const SodiumPoint &pt) {
    SIMPLE_NOTIFY_EV(ev_star_selected(pt));
}

void EventServer::NotifyStarLost(const FrameDroppedInfo &info) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("StarLost");

    ev << NV("Frame", info.frameNumber) << NV("Time", info.time, 3)
       << NV("StarMass", info.starMass, 0) << NV("SNR", info.starSNR, 2)
       << NV("HFD", info.starHFD, 2) << NV("AvgDist", info.avgDist, 2);

    if (info.starError)
        ev << NV("ErrorCode", info.starError);

    if (!info.status.IsEmpty())
        ev << NV("Status", info.status);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyGuidingStarted() {
    SIMPLE_NOTIFY_EV(ev_start_guiding());
}

void EventServer::NotifyGuidingStopped() { SIMPLE_NOTIFY("GuidingStopped"); }

void EventServer::NotifyPaused() { SIMPLE_NOTIFY_EV(ev_paused()); }

void EventServer::NotifyResumed() { SIMPLE_NOTIFY("Resumed"); }

void EventServer::NotifyGuideStep(const GuideStepInfo &step) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("GuideStep");

    ev << NV("Frame", step.frameNumber) << NV("Time", step.time, 3)
       << NVMount(step.mount) << NV("dx", step.cameraOffset.X, 3)
       << NV("dy", step.cameraOffset.Y, 3)
       << NV("RADistanceRaw", step.mountOffset.X, 3)
       << NV("DECDistanceRaw", step.mountOffset.Y, 3)
       << NV("RADistanceGuide", step.guideDistanceRA, 3)
       << NV("DECDistanceGuide", step.guideDistanceDec, 3);

    if (step.durationRA > 0) {
        ev << NV("RADuration", step.durationRA)
           << NV("RADirection",
                 step.mount->DirectionStr((GUIDE_DIRECTION)step.directionRA));
    }

    if (step.durationDec > 0) {
        ev << NV("DECDuration", step.durationDec)
           << NV("DECDirection",
                 step.mount->DirectionStr((GUIDE_DIRECTION)step.directionDec));
    }

    if (step.mount->IsStepGuider()) {
        ev << NV("Pos", step.aoPos);
    }

    ev << NV("StarMass", step.starMass, 0) << NV("SNR", step.starSNR, 2)
       << NV("HFD", step.starHFD, 2) << NV("AvgDist", step.avgDist, 2);

    if (step.starError)
        ev << NV("ErrorCode", step.starError);

    if (step.raLimited)
        ev << NV("RALimited", true);

    if (step.decLimited)
        ev << NV("DecLimited", true);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyGuidingDithered(double dx, double dy) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("GuidingDithered");
    ev << NV("dx", dx, 3) << NV("dy", dy, 3);

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifySetLockPosition(const SodiumPoint &xy) {
    if (m_eventServerClients.empty())
        return;

    do_notify(m_eventServerClients, ev_set_lock_position(xy));
}

void EventServer::NotifyLockPositionLost() {
    SIMPLE_NOTIFY("LockPositionLost");
}

void EventServer::NotifyLockShiftLimitReached() {
    SIMPLE_NOTIFY("LockPositionShiftLimitReached");
}

void EventServer::NotifyAppState() {
    if (m_eventServerClients.empty())
        return;

    do_notify(m_eventServerClients, ev_app_state());
}

void EventServer::NotifySettleBegin() { SIMPLE_NOTIFY("SettleBegin"); }

void EventServer::NotifySettling(double distance, double time,
                                 double settleTime, bool starLocked) {
    if (m_eventServerClients.empty())
        return;

    Ev ev(ev_settling(distance, time, settleTime, starLocked));

    Debug.Write(wxString::Format("evsrv: %s\n", ev.str()));

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifySettleDone(const wxString &errorMsg, int settleFrames,
                                   int droppedFrames) {
    if (m_eventServerClients.empty())
        return;

    Ev ev(ev_settle_done(errorMsg, settleFrames, droppedFrames));

    Debug.Write(wxString::Format("evsrv: %s\n", ev.str()));

    do_notify(m_eventServerClients, ev);
}

void EventServer::NotifyAlert(const wxString &msg, int type) {
    if (m_eventServerClients.empty())
        return;

    Ev ev("Alert");
    ev << NV("Msg", msg);

    wxString s;
    switch (type) {
        case wxICON_NONE:
        case wxICON_INFORMATION:
        default:
            s = "info";
            break;
        case wxICON_QUESTION:
            s = "question";
            break;
        case wxICON_WARNING:
            s = "warning";
            break;
        case wxICON_ERROR:
            s = "error";
            break;
    }
    ev << NV("Type", s);

    do_notify(m_eventServerClients, ev);
}

template <typename T>
static void NotifyGuidingParam(const EventServer::CliSockSet &clients,
                               const wxString &name, T val) {
    if (clients.empty())
        return;

    Ev ev("GuideParamChange");
    ev << NV("Name", name);
    ev << NV("Value", val);

    do_notify(clients, ev);
}

void EventServer::NotifyGuidingParam(const wxString &name, double val) {
    ::NotifyGuidingParam(m_eventServerClients, name, val);
}

void EventServer::NotifyGuidingParam(const wxString &name, int val) {
    ::NotifyGuidingParam(m_eventServerClients, name, val);
}

void EventServer::NotifyGuidingParam(const wxString &name, bool val) {
    ::NotifyGuidingParam(m_eventServerClients, name, val);
}

void EventServer::NotifyGuidingParam(const wxString &name,
                                     const wxString &val) {
    ::NotifyGuidingParam(m_eventServerClients, name, val);
}

void EventServer::NotifyConfigurationChange() {
    if (m_configEventDebouncer == nullptr ||
        m_configEventDebouncer->IsRunning())
        return;

    Ev ev("ConfigurationChange");
    do_notify(m_eventServerClients, ev);
    m_configEventDebouncer->StartOnce(0);
}
