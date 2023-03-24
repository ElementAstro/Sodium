#include <iostream>
#include <array>
#include <string>
#include <future>
#include <chrono>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <fitsio.h>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <map>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <unistd.h>
#endif

using namespace std;

namespace fs = filesystem;

/**
 * @brief 检查指定的可执行文件是否存在且可以执行。
 * 
 * @param file_name 文件名（不包含扩展名）。
 * @param file_ext 扩展名（包括点号）。
 * @return true 如果文件存在且可以执行。
 * @return false 如果文件不存在或不可执行。
 */
bool check_executable_file(const string& file_name, const string& file_ext) {
    // 构建文件路径
    #if defined(_WIN32)
        fs::path file_path = file_name + file_ext;
    #else
        fs::path file_path = file_name;
    #endif

    // 输出调试信息
    spdlog::debug("Checking file '{}'.", file_path.string());

    // 检查文件是否存在
    if (!fs::exists(file_path)) {
        spdlog::warn("The file '{}' does not exist.", file_path.string());
        return false;
    }

    // 检查文件权限（仅限 Linux）
    #if !defined(_WIN32)
        if (!fs::is_regular_file(file_path) || access(file_path.c_str(), X_OK) != 0) {
            spdlog::warn("The file '{}' is not a regular file or is not executable.", file_path.string());
            return false;
        }
    #endif

    // 检查文件是否为可执行文件（仅限 Windows）
    #if defined(_WIN32)
        if (!fs::is_regular_file(file_path) || !(GetFileAttributesA(file_path.c_str()) & FILE_ATTRIBUTE_DIRECTORY)) {
            spdlog::warn("The file '{}' is not a regular file or is not executable.", file_path.string());
            return false;
        }
    #endif

    spdlog::info("The file '{}' exists and is executable.", file_path.string());
    return true;
}

bool IsSubstring(const std::string& str, const std::string& sub) {
    return str.find(sub) != std::string::npos;
}

/**
 * @brief 使用 ASTAP 命令进行图像匹配，并返回匹配结果的输出。如果执行过程中出现错误，返回一个空字符串。
 * 
 * @param command ASTAP 命令的字符串。该参数必须提供。
 * @param ra 图像所在区域的赤经（度）。如果不需要指定
 * @param dec 图像所在区域的赤纬（度）。如果不需要指定
 * @param fov 图像视场（度）。如果不需要指定
 * @param timeout 执行命令的超时时间（秒数）。默认值为30秒。
 * @param update 是否在执行匹配前更新星表。默认为true。
 * @param image 需要匹配的图像文件路径。如果不需要指定，请传入一个空字符串。
 * @return 如果执行成功，返回匹配结果的输出；否则返回一个空字符串。
 */
string execute_astap_command(string command, double ra = 0.0, double dec = 0.0, double fov = 0.0, int timeout = 30, bool update = true, string image = "") {
    // 输出调试信息
    spdlog::debug("Executing command '{}' with RA={}, DEC={}, FOV={}, timeout={}s, image='{}'.", command, ra, dec, fov, timeout, image);

    // 构造命令行字符串
    if (ra != 0.0) {
        command += " -ra " + to_string(ra);
    }
    if (dec != 0.0) {
        command += " -spd " + to_string(dec + 90);
    }
    if (fov != 0.0) {
        command += " -fov " + to_string(fov);
    }
    if (!image.empty()) {
        command += " -f " + image;
    }
    if (update) {
        command += " -update ";
    }
    // 执行命令并等待结果
    array<char, 4096> buffer{};
    future<string> result = async(launch::async, [&]() -> string {
        // 创建子进程
        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            throw runtime_error("Error: failed to run command '" + command + "'.");
        }
        // 读取子进程的输出
        string output = "";
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            output += buffer.data();
        }
        // 关闭子进程并返回输出结果
        pclose(pipe);
        return output;
    });

    // 等待命令执行完成，或者超时
    auto start_time = chrono::system_clock::now();
    while (result.wait_for(chrono::seconds(1)) != future_status::ready) {
        auto elapsed_time = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start_time).count();
        if (elapsed_time > timeout) {
            spdlog::error("Error: command '{}' timed out after {} seconds.", command, timeout);
            return "";
        }
    }
    // 返回命令执行结果，并输出调试信息
    auto output = result.get();
    spdlog::debug("Command '{}' returned: {}", command, output);
    return output;
}

/**
 * @brief Solve FITS header and return solved RA/DEC, rotation, and focal length.
 * 
 * @param image The name of the input FITS image file.
 * 
 * @return A map structure containing the following keys:
 * - message: A string describing the solving result.
 * - ra: The right ascension in degrees (string).
 * - dec: The declination in degrees (string).
 * - rotation: The rotation angle (string).
 * - focal_length: The average focal length in millimeters (string).
 */
map<string, string> read_astap_result(const string& image) {
    map<string, string> ret_struct;

    // 打开 FITS 文件并读取头信息
    fitsfile *fptr;
    int status = 0;
    fits_open_file(&fptr, image.c_str(), READONLY, &status);
    if (status != 0) {
        ret_struct["message"] = "Error: cannot open FITS file '" + image + "'.";
        spdlog::error(ret_struct["message"]);
        return ret_struct;
    }

    double solved_ra, solved_dec, x_pixel_arcsec, y_pixel_arcsec, rotation, x_pixel_size, y_pixel_size;
    bool data_get_flag = false;
    char comment[FLEN_COMMENT];

    // 读取头信息中的关键字
    fits_read_key(fptr, TDOUBLE, "CRVAL1", &solved_ra, comment, &status);
    fits_read_key(fptr, TDOUBLE, "CRVAL2", &solved_dec, comment, &status);
    fits_read_key(fptr, TDOUBLE, "CDELT1", &x_pixel_arcsec, comment, &status);
    fits_read_key(fptr, TDOUBLE, "CDELT2", &y_pixel_arcsec, comment, &status);
    fits_read_key(fptr, TDOUBLE, "CROTA1", &rotation, comment, &status);
    fits_read_key(fptr, TDOUBLE, "XPIXSZ", &x_pixel_size, comment, &status);
    fits_read_key(fptr, TDOUBLE, "YPIXSZ", &y_pixel_size, comment, &status);

    // 检查是否成功读取关键字
    if (status == 0) {
        data_get_flag = true;
    } else {
        spdlog::warn("Warning: cannot read some FITS header keywords.");
        status = 0;
    }

    // 关闭 FITS 文件
    fits_close_file(fptr, &status);

    // 构造返回结果
    if (data_get_flag) {
        ret_struct["message"] = "Solve success";
        ret_struct["ra"] = to_string(solved_ra);
        ret_struct["dec"] = to_string(solved_dec);
        ret_struct["rotation"] = to_string(rotation);
        if (x_pixel_size > 0.0 && y_pixel_size > 0.0) {
            double x_focal_length = x_pixel_size / x_pixel_arcsec * 206.625;
            double y_focal_length = y_pixel_size / y_pixel_arcsec * 206.625;
            double avg_focal_length = (x_focal_length + y_focal_length) / 2.0;
            ret_struct["focal_length"] = to_string(avg_focal_length);
            // 调试输出
            spdlog::debug("avg_focal_length: {}", avg_focal_length);
        }
    } else {
        ret_struct["message"] = "Solve failed";
    }
    
    // 最终输出
    spdlog::info("Function solve_fits_header result: {}", ret_struct["message"]);

    return ret_struct;
}

/**
 * @brief 使用 ASTAP 进行图像匹配，并返回匹配结果。如果匹配成功，返回包含星表数据的映射；否则返回一个错误消息。
 * 
 * @param ra 图像所在区域的赤经（度）。如果不需要指定
 * @param dec 图像所在区域的赤纬（度）。如果不需要指定
 * @param fov 图像视场（度）。如果不需要指定
 * @param timeout 执行命令的超时时间（秒数）。默认值为30秒。
 * @param update 是否在执行匹配后更新图片。默认为true。
 * @param image 需要匹配的图像文件路径。该参数必须提供。
 * @return 如果匹配成功，返回包含星表数据的映射；否则返回一个错误消息。
 */
extern "C" map<string, string> run_astap(double ra = 0.0, double dec = 0.0, double fov = 0.0, int timeout = 30, bool update = true, string image = ""){
    map<string, string> ret_struct;
    if(!check_executable_file("/usr/bin/astap","") && !check_executable_file("/usr/local/bin/astap","")){
        spdlog::debug("No Astap solver engine found , please install before trying to solve an image");
        ret_struct["message"] = "No solver found!";
        return ret_struct;
    }
    
    string result = execute_astap_command("astap", ra, dec, fov, timeout,update, image);
    if(IsSubstring(result,"Solution found:")){
        spdlog::info("Solved successfully");
        ret_struct = read_astap_result(image);
    }
    else{
        spdlog::error("Failed to solve the image");
        ret_struct["message"] = "Failed to solve the image";
    }
    return ret_struct;
}

extern "C" map<string, string> run_astrometry(string image, string ra = "", string dec = "", double radius = 0.0, int downsample = 0,
                  vector<int> depth = {}, double scale_low = 0.0, double scale_high = 0.0, int width = 0, int height = 0,
                  string scale_units = "degwidth", bool overwrite = true, bool no_plot = true, bool verify = false,
                  bool debug = false, int timeout = 30, bool resort = false, bool _continue = false, bool no_tweak = false){

    map<string, string> ret_struct;
    if(!check_executable_file("/usr/bin/","solve-field") && !check_executable_file("/usr/local/bin/solve-field","")){
        spdlog::debug("No Astap solver engine found , please install before trying to solve an image");
        ret_struct["message"] = "No solver found!";
        return ret_struct;
    }

    vector<string> command = {"solve-field", image};

    if (!ra.empty()) {
        command.push_back("--ra");
        command.push_back(ra);
    }

    if (!dec.empty()) {
        command.push_back("--dec");
        command.push_back(dec);
    }

    if (radius != 0.0) {
        command.push_back("--radius");
        command.push_back(to_string(radius));
    }

    if (downsample != 0) {
        command.push_back("--downsample");
        command.push_back(to_string(downsample));
    }

    if (!depth.empty()) {
        command.push_back("--depth");
        string depth_str = "";
        for (int i = 0; i < depth.size(); ++i) {
            depth_str += to_string(depth[i]);
            if (i != depth.size() - 1) {
                depth_str += ",";
            }
        }
        command.push_back(depth_str);
    }

    if (scale_low != 0.0) {
        command.push_back("--scale-low");
        command.push_back(to_string(scale_low));
    }

    if (scale_high != 0.0) {
        command.push_back("--scale-high");
        command.push_back(to_string(scale_high));
    }

    if (width != 0) {
        command.push_back("--width");
        command.push_back(to_string(width));
    }

    if (height != 0) {
        command.push_back("--height");
        command.push_back(to_string(height));
    }

    if (!scale_units.empty()) {
        command.push_back("--scale-units");
        command.push_back(scale_units);
    }

    if (overwrite) {
        command.push_back("--overwrite");
    }

    if (no_plot) {
        command.push_back("--no-plot");
    }

    if (verify) {
        command.push_back("--verify");
    }

    if (resort) {
        command.push_back("--resort");
    }

    if (_continue) {
        command.push_back("--continue");
    }

    if (no_tweak) {
        command.push_back("--no-tweak");
    }

    string cmd_str = "";
    for (string c : command) {
        cmd_str += c + " ";
    }

    // 执行命令并等待结果
    array<char, 4096> buffer{};
    future<string> result = async(launch::async, [&]() -> string {
        // 创建子进程
        FILE *pipe = popen(cmd_str.c_str(), "r");
        if (!pipe) {
            throw runtime_error("Error: failed to run command '" + cmd_str + "'.");
        }
        // 读取子进程的输出
        string output = "";
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            output += buffer.data();
        }
        // 关闭子进程并返回输出结果
        pclose(pipe);
        return output;
    });

    // 等待命令执行完成，或者超时
    auto start_time = chrono::system_clock::now();
    while (result.wait_for(chrono::seconds(1)) != future_status::ready) {
        auto elapsed_time = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - start_time).count();
        if (elapsed_time > timeout) {
            spdlog::error("Error: command '{}' timed out after {} seconds.", cmd_str, timeout);
            return ret_struct;
        }
    }
    // 返回命令执行结果，并输出调试信息
    auto output = result.get();
    spdlog::debug("Command '{}' returned: {}", cmd_str, output);

    std::vector<std::string> lines; // 存放分割后的字符串

    // 将字符串按照换行符分割，存入 vector 中
    std::string delimiter = "\n";
    size_t pos = 0;
    std::string token;
    while ((pos = output.find(delimiter)) != std::string::npos) {
        token = output.substr(0, pos);
        lines.push_back(token);
        output.erase(0, pos + delimiter.length());
    }

    for (string item : lines) {
            if (item.find("Field center: (RA H:M:S, Dec D:M:S) = ") != -1) {
                size_t pos1 = item.find("Field center: (RA H:M:S, Dec D:M:S) = ");
                string ra_dec;
                string r = "( )";
                if (pos1 != string::npos) {
                    size_t pos2 = item.find(".", pos1);
                    if (pos2 != string::npos) {
                        pos1 += strlen("Field center: (RA H:M:S, Dec D:M:S) = ");
                        ra_dec = item.substr(pos1, pos2 - pos1);
                        size_t pos3 = ra_dec.find_first_of(r);
                        while (pos3 != string::npos) {
                            ra_dec.replace(pos3, 1, "");
                            pos3 = ra_dec.find_first_of(r);
                        }
                        cout << ra_dec << endl;
                    }
                }
                size_t pos = ra_dec.find(",");
                ret_struct["ra"] = ra_dec.substr(0, pos);
                ret_struct["dec"] = ra_dec.substr(pos + 1);
            }
            
            if (item.find("Field size: ") != -1) {
                string fov_;
                string r = "' ";
                size_t pos1 = item.find("Field size: ");
                if (pos1 != string::npos) {
                    size_t pos2 = item.find(".", pos1);
                    if (pos2 != string::npos) {
                        pos1 += strlen("Field size: ");
                        string fov = item.substr(pos1, pos2 - pos1);
                        size_t pos3 = fov.find_first_of(r);
                        while (pos3 != string::npos) {
                            fov.replace(pos3, 1, "");
                            pos3 = fov.find_first_of(r);
                        }
                    }
                }
                size_t pos = fov_.find("x");
                ret_struct["fov_x"] = fov_.substr(0, pos);
                ret_struct["fov_y"] = fov_.substr(pos + 1);
            }
            
            if (item.find("Field rotation angle: up is ") != -1) {
                string r = " degrees E of N";

                size_t pos1 = item.find("Field rotation angle: up is ");
                if (pos1 != string::npos) {
                    size_t pos2 = item.find(r, pos1);
                    if (pos2 != string::npos) {
                        pos1 += strlen("Field rotation angle: up is ");
                        ret_struct["rotation"] =  item.substr(pos1, pos2 - pos1);
                    }
                }
            }
        }

        if (ret_struct["ra"].empty() || ret_struct["dec"].empty()) {
            ret_struct["message"] = "Solve failed";
            return ret_struct;
        }

    return ret_struct;
}

int main() {
    // 示例调用
    string result = execute_astap_command("astap", 12.34, 56.78, 2.0, 30, "/path/to/image.fits");
    cout << result << endl;
    return 0;
}
