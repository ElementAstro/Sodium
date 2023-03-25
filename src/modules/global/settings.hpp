#include <iostream>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h> // 添加spdlog头文件

namespace fs = boost::filesystem;
using namespace std;

class GlobalSetting {
    public:
    /**
    * 构造函数，根据设备名加载配置信息
    * @param device_list_name 设备名
    */
    GlobalSetting(const string& device_list_name) {
        load_setting(device_list_name);
    }

    /**
     * 加载设备名对应的配置信息，若不存在则初始化默认信息并保存。
     * @param device_list_name 设备名
     */
    void load_setting(const string& device_list_name) {
        fs::path temp_file_path("./server/data/global_setting");
        temp_file_path /= device_list_name + ".json";

        global_setting_file = temp_file_path.string();
        this->device_list_name = device_list_name;

        if (fs::exists(temp_file_path)) {
            try {
                load_global_data(temp_file_path);
            }
            catch(const exception& e) {
                spdlog::error(e.what()); // 使用spdlog记录错误信息
                cerr << "Error in loading existing setting file. Load as Reset to empty!" << endl;
                init_default_global_data();
            }
        }
        else {
            if (!fs::exists(temp_file_path.parent_path())) {
                fs::create_directories(temp_file_path.parent_path());
            }
            init_default_global_data();
            save_global_data(temp_file_path.string());
        }
    }

    /**
     * 获取指定参数名对应的配置信息树。
     * @param parameter_name 参数名
     * @return 配置信息树
     */
    boost::property_tree::ptree get_global_setting(const string& parameter_name) const {
        if (global_data.count(parameter_name)) {
            return global_data.get_child(parameter_name);
        }
        else {
            return boost::property_tree::ptree();
        }
    }

    /**
     * 设置指定参数名对应的配置信息树
     * @param parameter_name 参数名
     * @param setting_dict 配置信息树
     */
    void set_global_setting(const string& parameter_name, const boost::property_tree::ptree& setting_dict) {
        if (global_data.count(parameter_name)) {
            auto& parameter_tree = global_data.get_child(parameter_name);
            for (const auto& item : setting_dict) {
                if (parameter_tree.count(item.first)) {
                    parameter_tree.put(item.first, item.second.data());
                }
            }
            save_global_data(global_setting_file);
            spdlog::info("Updated"); // 使用spdlog记录信息
        }
        else {
            spdlog::error("Parameter {} is incorrect", parameter_name); // 使用spdlog记录错误信息
        }
    }
    private:
    /**
    * 初始化默认的配置信息
    */
    void init_default_global_data() {
    global_data.put("geo_location.longitude", 0);
    global_data.put("geo_location.latitude", 0);
    global_data.put("geo_location.height", 0);

        global_data.put("meridian_flip.before_merdian", 2);
        global_data.put("meridian_flip.after_merdian", 2);
        global_data.put("meridian_flip.recenter_after_flip", true);
        global_data.put("meridian_flip.autofocus_after_flip", false);
        global_data.put("meridian_flip.settle_time_after", 30);

        for (int i = 0; i < 8; i++) {
            global_data.add("filter_setting.filter_info.filter_number", "");
            global_data.add("filter_setting.filter_info.filter_name", "filter" + to_string(i+1));
            global_data.add("filter_setting.filter_info.focus_offset", 0);
            global_data.add("filter_setting.filter_info.af_exposure_time", 1);
        }

        global_data.put("autofocus.use_filter_offset", true);
        global_data.put("autofocus.step_size", 50);
        global_data.put("autofocus.initial_steps", 5);
        global_data.put("autofocus.default_exposure_time", 1);
        global_data.put("autofocus.retry_times", 1);
        global_data.put("autofocus.each_step_exposure", 1);

        global_data.put("plate_solve.use", "astap");
        global_data.put("plate_solve.exposure_time", 2);
        global_data.put("plate_solve.use_filter", nullptr);
        global_data.put("plate_solve.downsample", nullptr);

        global_data.put("telescope_info.name", nullptr);
        global_data.put("telescope_info.aperture", 80);
        global_data.put("telescope_info.focal_length", 480);
        global_data.put("telescope_info.guider_aperture", 60);
        global_data.put("telescope_info.guider_focal_length", 240);

        global_data.put("camera_info_get", false);
        global_data.put("camera_info.CCD_MAX_X", nullptr);
        global_data.put("camera_info.CCD_MAX_Y", nullptr);
        global_data.put("camera_info.CCD_PIXEL_SIZE", nullptr);

        global_data.put("guider_camera_info_get", false);
        global_data.put("guider_camera_info.CCD_MAX_X", nullptr);
        global_data.put("guider_camera_info.CCD_MAX_Y", nullptr);
        global_data.put("guider_camera_info.CCD_PIXEL_SIZE", nullptr);
    }

    /**
     * 保存配置信息到指定文件
     * @param file_path 文件路径
     */
    void save_global_data(const string& file_path) {
        ofstream output_file(file_path);
        boost::property_tree::write_json(output_file, global_data);
        output_file.close();
    }

    /**
     * 从指定文件加载配置信息到内存
     * @param file_path 文件路径
     */
    void load_global_data(const fs::path& file_path) {
        ifstream input_file(file_path.string());
        boost::property_tree::read_json(input_file, global_data);
        input_file.close();
    }
    private:
    string global_setting_file;
    string device_list_name;
    boost::property_tree::ptree global_data;
};

int main() {
    GlobalSetting setting("device1");
    auto parameter = setting.get_global_setting("meridian_flip");
    spdlog::info("meridian_flip.before_merdian: {}", parameter.get<string>("before_merdian")); // 使用spdlog记录信息
    parameter.put("before_merdian", 5);
    setting.set_global_setting("meridian_flip", parameter);
    return 0;
}