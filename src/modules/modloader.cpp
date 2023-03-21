/*
 * modloader.cpp
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
 
Date: 2023-3-16
 
Description: Chai Script Loader and Lua Script Loader
 
**************************************************/

#include "lightguider.h"
#include "modloader.h"

#include "plugins/cmd_manager.hpp"
#include "plugins/file_monitor.hpp"
#include "plugins/time.hpp"
#include "plugins/proc_manager.hpp"

#include "configor/configor.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

#include <openssl/md5.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <curl/curl.h>

namespace fs = std::filesystem;
namespace pt = boost::property_tree;

namespace LightGuider{

    ModuleLoader module_loader;
    
    configor::json::value iterator_modules_dir() {
        // 定义modules目录路径
        fs::path modules_dir{"modules"};
        // 定义一个空的json对象
        configor::json::value config;
        // 判断modules目录是否存在，不存在则返回错误信息
        try {
            if (!fs::exists(modules_dir) || !fs::is_directory(modules_dir)) {
                spdlog::error("Error: modules folder not found!");
                config["error"] = "Error: modules folder not found!";
                return config;
            }
            // 遍历modules目录下的所有子目录
            for (auto& dir : fs::directory_iterator(modules_dir)) {
                // 如果当前目录是子目录
                if (fs::is_directory(dir)) {
                    // 获取子目录下的info.json文件路径
                    fs::path info_file = dir.path() / "info.json";
                    // 如果info.json文件存在
                    if (fs::exists(info_file)) {
                        // 在json对象中添加子目录路径和info.json文件路径
                        config[dir.path().string()]["path"] = dir.path().string();
                        config[dir.path().string()]["config"] = info_file.string();
                        // 调试输出信息
                        spdlog::debug("Module found: {}, config file: {}", dir.path().string(), info_file.string());
                    }
                }
            }
        } catch (const std::exception& e) { // 捕获异常
            spdlog::error("Failed to iterate modules directory: {}", e.what());
            config["error"] = "Failed to iterate modules directory";
        }
        // 返回json对象
        return config;
    }

    ModuleLoader::ModuleLoader(){
        L_ = luaL_newstate();
        luaL_openlibs(L_);
        spdlog::info("Lua script manager loaded successfully.");
        
        Py_Initialize();
        spdlog::info("Python script manager loaded successfully.");
    }

    ModuleLoader::~ModuleLoader(){
        for (auto& entry : handles_) {
            dlclose(entry.second);
        }
        lua_close(L_);

        for (auto& [scriptName, moduleObj] : python_modules_) {
            Py_DECREF(moduleObj);
        }
        Py_Finalize();
    }

    bool ModuleLoader::LoadModule(const std::string& path, const std::string& name) {
        // 通过dlopen函数加载动态库
        void* handle = nullptr;
        try {
            handle = dlopen(path.c_str(), RTLD_NOW);
        } catch (const std::exception& e) { // 捕获异常
            spdlog::error("Failed to load library {}: {}", path, e.what());
            return false;
        }
        // 如果加载失败，则输出错误信息，并返回false
        if (!handle) {
            spdlog::error("Failed to load library {}: {}", path, dlerror());
            return false;
        }
        // 将动态库句柄保存到handles_中
        handles_[name] = handle;
        return true;
    }

    /**
     * @brief 加载Lua模块
     * @param moduleName 模块名
     * @param fileName 模块文件名
     * @return 是否加载成功
     */
    bool ModuleLoader::loadLuaModule(const std::string& moduleName, const std::string& fileName) {
        try {
            // 打开文件
            std::ifstream file(fileName);
            if (!file.is_open()) {
                spdlog::error("Failed to open file: {}", fileName); 
                return false;
            }
            // 读取文件内容
            std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            // 执行脚本
            int result = luaL_dostring(L_, script.c_str());
            if (result != LUA_OK) {
                spdlog::error("Error loading module {}: {}", moduleName, lua_tostring(L_, -1)); 
                lua_pop(L_, 1);
                return false;
            }
            // 获取模块中的所有函数
            std::vector<std::string> functionList;
            lua_pushglobaltable(L_);
            lua_pushnil(L_);
            while (lua_next(L_, -2) != 0) {
                if (lua_isfunction(L_, -1)) {
                    const char* name = lua_tostring(L_, -2);
                    if (name) {
                        functionList.push_back(name);
                    }
                }
                lua_pop(L_, 1);
            }
            lua_pop(L_, 1);
            lua_modules_[moduleName] = functionList; // 将模块名和函数列表存入map中
            spdlog::info("Loaded module {}", moduleName); 
            return true;
        } catch (const std::exception& e) { // 捕获异常
            spdlog::error("Failed to load module {}: {}", moduleName, e.what());
            return false;
        }
    }

    /**
     * @brief 根据函数名执行Lua函数
     * @param funcName 函数名
     * @return 执行结果
     */
    std::string ModuleLoader::LoadAndRunLuaFunction(const std::string& funcName) {
        if (lua_modules_.empty()) {
            spdlog::warn("No Lua module loaded"); 
            return "";
        }
        // 查找函数名
        const std::string* moduleName = nullptr;
        const std::vector<std::string>* functionList = nullptr;
        for (const auto& pair: lua_modules_) {
            auto iter = std::find(pair.second.cbegin(), pair.second.cend(), funcName);
            if (iter != pair.second.cend()) {
                moduleName = &pair.first;
                functionList = &pair.second;
                break;
            }
        }
        if (!moduleName || !functionList) {
            spdlog::warn("No Lua function found: {}", funcName); 
            return "";
        }
        // 获取函数
        lua_getglobal(L_, moduleName->c_str());
        lua_getfield(L_, -1, funcName.c_str());
        lua_remove(L_, -2);
        if (!lua_isfunction(L_, -1)) {
            spdlog::error("Invalid Lua function: {}.{}", moduleName->c_str(), funcName.c_str()); 
            lua_pop(L_, 1);
            return "";
        }
        // 多线程执行
        std::promise<int> promise;
        std::future<int> future = promise.get_future();
        std::thread thread([this, &promise]() {
            int result = lua_pcall(L_, 0, 1, 0);
            promise.set_value(result);
        });
        thread.detach();
        // 等待执行结束
        int result = future.get();
        if (result != LUA_OK) {
            spdlog::error("Error calling Lua function {}.{}: {}", moduleName->c_str(), funcName.c_str(), lua_tostring(L_, -1));
            lua_pop(L_, 1);
            return "";
        }
        // 获取返回值
        std::string ret = lua_tostring(L_, -1);
        lua_pop(L_, 1);
        return ret;
    }

    bool ModuleLoader::LoadPythonScript(const std::string& scriptName){
        PyObject* pModule = PyImport_ImportModule(scriptName.c_str());
        if (!pModule) {
            spdlog::error("Failed to load Python module: {}",scriptName);
            return false;
        }
        python_modules_[scriptName] = pModule;
        return true;
    }

    void ModuleLoader::UnloadPythonScript(const std::string& scriptName) {
        auto iter = python_modules_.find(scriptName);
        if (iter != python_modules_.end()) {
            Py_DECREF(iter->second);
            python_modules_.erase(iter);
        }
    }

    std::vector<std::string> ModuleLoader::getPythonFunctions(const std::string& scriptName) {
        std::vector<std::string> result;

        auto iter = python_modules_.find(scriptName);
        if (iter == python_modules_.end()) {
            std::cerr << "Script not found: " << scriptName << std::endl;
            return result;
        }

        PyObject* pDict = PyModule_GetDict(iter->second);
        PyObject* pKey, * pValue;
        Py_ssize_t pos = 0;

        while (PyDict_Next(pDict, &pos, &pKey, &pValue)) {
            if (PyCallable_Check(pValue)) {
                std::string functionName = PyUnicode_AsUTF8(pKey);
                result.push_back(functionName);
            }
        }

        return result;
    }

    // 使用 libcurl 库下载文件，并保存到指定路径
    // url：下载文件的 URL
    // filename：保存文件的路径
    // 返回值：true 表示下载成功，false 表示下载失败
    // 调用方法： downloadFile("http://example.com/file.txt", "file.txt")
    bool downloadFile(const char* url, const char* filename) {
        try {
            CURL* curl = curl_easy_init();
            if (!curl) {
                spdlog::error("Failed to initialize curl");
                return false;
            }
            curl_easy_setopt(curl, CURLOPT_URL, url);
            CURLcode result = curl_easy_perform(curl);
            if (result != CURLE_OK) {
                spdlog::error("Failed to download {}: {}", url, curl_easy_strerror(result));
                curl_easy_cleanup(curl);
                return false;
            }
            std::vector<char> contents;
            curl_off_t cl;
            result = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
            if (result == CURLE_OK && cl > 0) {
                contents.reserve(cl);
            }
            int http_code;
            result = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (result == CURLE_OK && http_code == 200) {
                auto write_callback = [](char* buffer, size_t size, size_t nitems, void* userdata) -> size_t {
                    auto& data = *static_cast<std::vector<char>*>(userdata);
                    const size_t bytes = size * nitems;
                    data.insert(data.end(), buffer, buffer + bytes);
                    return bytes;
                };
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contents);
                result = curl_easy_perform(curl);
            }
            if (result != CURLE_OK) {
                spdlog::error("Failed to download {}: {}", url, curl_easy_strerror(result));
                curl_easy_cleanup(curl);
                return false;
            }
            std::ofstream outfile(filename, std::ios::binary);
            if (!outfile) {
                spdlog::error("Failed to open file {}", filename);
                curl_easy_cleanup(curl);
                return false;
            }
            outfile.write(contents.data(), contents.size());
            outfile.close();
            spdlog::info("Downloaded {} to {}", url, filename);
            curl_easy_cleanup(curl);
            return true;
        } catch (std::exception& e) {
            spdlog::error("Exception caught while downloading {}: {}", url, e.what());
            return false;
        } catch (...) {
            spdlog::error("Unknown exception caught while downloading {}", url);
            return false;
        }
    }

    // 计算文件的MD5值
    // 调用：int major, minor, patch;
    //      read_version_info("version.ini", major, minor, patch);
    //      std::cout << "Version: " << major << "." << minor << "." << patch << std::endl;
    std::string get_file_md5(const std::string& file_path) {
        try {
            // 初始化MD5上下文和存储MD5值的数组
            MD5_CTX ctx;
            unsigned char md[MD5_DIGEST_LENGTH];
            // 定义读取文件的缓冲区和长度
            char buf[1024];
            size_t len;
            // 打开二进制文件输入流
            std::ifstream ifs(file_path, std::ios::binary);
            if (!ifs.good()) {
                throw std::runtime_error("Failed to open file: " + file_path);
            }
            // 初始化MD5上下文
            MD5_Init(&ctx);
            // 循环读取文件内容，并更新MD5上下文
            while (ifs.good()) {
                ifs.read(buf, sizeof(buf));
                len = ifs.gcount();
                MD5_Update(&ctx, buf, len);
            }
            // 关闭文件输入流并计算最终的MD5值
            ifs.close();
            MD5_Final(md, &ctx);
            // 将MD5值转换为十六进制字符串并返回
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
                ss << std::setw(2) << static_cast<unsigned int>(md[i]);
            }
            std::string md5 = ss.str();
            spdlog::info("MD5 of file {}: {}", file_path, md5);
            return md5;
        } catch (std::exception& e) {
            spdlog::error("Exception caught while getting MD5 of file {}: {}", file_path, e.what());
            return "";
        } catch (...) {
            spdlog::error("Unknown exception caught while getting MD5 of file {}", file_path);
            return "";
        }
    }

    // 读取INI格式的版本信息
    void read_version_info(const std::string& file_path, int& major, int& minor, int& patch) {
        // 定义属性树对象和读取INI格式文件
        pt::ptree pt;
        pt::ini_parser::read_ini(file_path, pt);
        // 从属性树对象中获取版本信息
        major = pt.get<int>("version.major");
        minor = pt.get<int>("version.minor");
        patch = pt.get<int>("version.patch");
    }

    // 读取文件并进行MD5校验
    // 调用：verify_file_md5(file_path, given_md5);
    bool verify_file_md5(const std::string& file_path, const std::string& md5) {
        // 计算文件的MD5值
        std::string file_md5 = get_file_md5(file_path);
        // 对比计算出的MD5值和给定的MD5值是否相等
        return (file_md5 == md5);
    }

}