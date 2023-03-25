#include <iostream>
#include <vector>
#include <libnova/julian_day.h>
#include <libnova/solar.h>
#include <libnova/mars.h>
#include <libnova/venus.h>
#include <libnova/mercury.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/precession.h>
#include <chrono>
#include <ctime>

using namespace std;

// 计算八大行星和太阳、月球的J2000坐标系位置
vector<ln_equ_posn> calc_planet_positions(double jd) {
    vector<ln_equ_posn> j2000_positions;

    // 太阳在J2000坐标系中的位置
    ln_equ_posn sun_j2000_posn;
    ln_get_solar_equ_coords(jd, &sun_j2000_posn);
    j2000_positions.push_back(sun_j2000_posn);

    // 八大行星在J2000坐标系中的位置
    ln_equ_posn posn;
    ln_get_mars_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_venus_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_mercury_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_jupiter_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_saturn_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_uranus_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    ln_get_neptune_equ_coords(jd, &posn);
    j2000_positions.push_back(posn);

    return j2000_positions;
}
// 将J2000坐标系中的赤道坐标转换为JNow坐标系中的赤道坐标
ln_equ_posn convert_j2000_to_jnow(ln_equ_posn j2000_posn, double jd_now) {
    // 计算J2000和JNow坐标系之间的差异
    ln_equ_prec_values equ_prec = ln_get_equ_prec_values();
    ln_get_equ_prec(jd_now, &equ_prec);

    // 将J2000坐标系中的赤道坐标转换为JNow坐标系中的赤道坐标
    ln_equ_posn jnow_posn;
    jnow_posn.ra = j2000_posn.ra + equ_prec.d_alpha;
    jnow_posn.dec = j2000_posn.dec + equ_prec.d_delta;
    return jnow_posn;
}

int main() {
    // 获取当前时间
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);

    // 计算儒略日
    int year = localtime(&t)->tm_year + 1900;
    int month = localtime(&t)->tm_mon + 1;
    int day = localtime(&t)->tm_mday;
    double jd = ln_get_julian_day(year, month, day, 12, 0, 0);  // 假设当前时间是中午

    // 计算八大行星和太阳、月球在J2000坐标系中的位置
    vector<ln_equ_posn> j2000_positions = calc_planet_positions(jd);

    // 将所有天体的J2000坐标系中的赤道坐标转换为JNow坐标系中的赤道坐标
    double jd_now = ln_get_julian_from_sys();
    vector<ln_equ_posn> jnow_positions;
    for (int i = 0; i < j2000_positions.size(); i++) {
        jnow_positions.push_back(convert_j2000_to_jnow(j2000_positions[i], jd_now));
    }

    // 输出结果
    char str[100];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", localtime(&t));
    cout << "当前时间：" << str << endl;

    cout << "太阳在J2000坐标系中的位置：" << endl;
    cout << "赤经：" << j2000_positions[0].ra << endl;
    cout << "赤纬：" << j2000_positions[0].dec << endl;

    cout << "太阳在JNow坐标系中的位置：" << endl;
    cout << "赤经：" << jnow_positions[0].ra << endl;
    cout << "赤纬：" << jnow_positions[0].dec << endl;

    cout << "月球在J2000坐标系中的位置：" << endl;
    cout << "赤经：" << j2000_positions[1].ra << endl;
    cout << "赤纬：" << j2000_positions[1].dec << endl;

    cout << "月球在JNow坐标系中的位置：" << endl;
    cout << "赤经：" << jnow_positions[1].ra << endl;
    cout << "赤纬：" << jnow_positions[1].dec << endl;

    for (int i = 2; i < j2000_positions.size(); i++) {
        string planet_name;
        switch (i) {
          case 2: planet_name = "火星"; break;
          case 3: planet_name = "金星"; break;
          case 4: planet_name = "水星"; break;
          case 5: planet_name = "木星"; break;
          case 6: planet_name = "土星"; break;
          case 7: planet_name = "天王星"; break;
          case 8: planet_name = "海王星"; break;
        }
        cout << planet_name << "在J2000坐标系中的位置：" << endl;
        cout << "赤经：" << j2000_positions[i].ra << endl;
        cout << "赤纬：" << j2000_positions[i].dec << endl;

        ln_equ_posn jnow_posn = convert_j2000_to_jnow(j2000_positions[i], jd_now);
        cout << planet_name << "在JNow坐标系中的位置：" << endl;
        cout << "赤经：" << jnow_posn.ra << endl;
        cout << "赤纬：" << jnow_posn.dec << endl;
    }

    return 0;
}
