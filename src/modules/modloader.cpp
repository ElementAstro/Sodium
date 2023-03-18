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
    
    configor::json::value iterator_modules_dir(){
        fs::path modules_dir{"modules"};
        configor::json::value config;
        if (!fs::exists(modules_dir) || !fs::is_directory(modules_dir))
        {
            spdlog::error("Error: modules folder not found!");
            config["error"] = "Error: modules folder not found!";
            return config;
        }
        for (auto& dir : fs::directory_iterator(modules_dir))
        {
            if (fs::is_directory(dir))
            {
                fs::path info_file = dir.path().string() + "/info.json";
                std::cout << info_file.filename();
                std::cout << info_file;
                if (fs::exists(info_file))
                {
                    std::cout << "a";
                    config[dir.path().string()]["path"] = dir.path().string();
                    config[dir.path().string()]["config"] = info_file.string();
                }
            }
        }
        return config;
    }

    ModuleLoader::ModuleLoader(){
        L_ = luaL_newstate();
        luaL_openlibs(L_);
        
        try{
            // 初始化chai_modules_，使用std::make_unique创建unique_ptr
            chai_modules_ = std::make_unique<chaiscript::ChaiScript>();
            // 将标准库添加到chai_modules_中
            chaiscript::ModulePtr stdlib = chaiscript::Std_Lib::library();
            chai_modules_->add(stdlib);
        }catch (const chaiscript::exception::eval_error& e) {
            spdlog::error("Failed to init the chaiscript loader");
        }
    }

    ModuleLoader::~ModuleLoader(){
        for (auto& entry : handles_) {
            dlclose(entry.second);
        }
        lua_close(L_);
    }

    bool ModuleLoader::LoadModule(const std::string& path, const std::string& name) {
        void* handle = dlopen(path.c_str(), RTLD_NOW);
        if (!handle) {
            spdlog::error("Failed to load library {}: {}", path, dlerror());
            return false;
        }
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
    }

    // 加载脚本，返回值表示是否加载成功
    bool ModuleLoader::LoadChaiScript(const std::string& filename) {
         try {
            // 读取并执行脚本文件
            chai_modules_->eval_file(filename);
            // 更新函数和变量列表
        } catch (const chaiscript::exception::eval_error& e) {
            spdlog::error("Failed to load script: {}", e.what());
            return false;
        }
        return true;
    }

    // 使用 libcurl 库下载文件，并保存到指定路径
    // url：下载文件的 URL
    // filename：保存文件的路径
    // 返回值：true 表示下载成功，false 表示下载失败
    // 调用方法： downloadFile("http://example.com/file.txt", "file.txt")
    bool downloadFile(const char* url, const char* filename) {
        // 初始化 curl
        CURL* curl = curl_easy_init();
        if (!curl) {
            spdlog::error("Failed to initialize curl");
            return false;
        }
        // 设置下载 URL
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // 执行下载操作
        CURLcode result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            spdlog::error("Failed to download {}: {}", url, curl_easy_strerror(result));
            curl_easy_cleanup(curl);
            return false;
        }
        // 获取文件内容
        std::vector<char> contents;
        curl_off_t cl;
        result = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
        if (result == CURLE_OK && cl > 0) {
            contents.reserve(cl);
        }
        int http_code;
        result = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (result == CURLE_OK && http_code == 200) {
            // 设置写入回调函数，将下载的数据存储到 contents 中
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
        // 保存文件
        std::ofstream outfile(filename, std::ios::binary);
        if (!outfile) {
            spdlog::error("Failed to open file {}", filename);
            curl_easy_cleanup(curl);
            return false;
        }
        outfile.write(contents.data(), contents.size());
        outfile.close();
        // 输出下载成功信息
        spdlog::info("Downloaded {} to {}", url, filename);
        // 释放 curl 资源
        curl_easy_cleanup(curl);
        // 返回下载成功
        return true;
    }

    // 计算文件的MD5值
    // 调用：int major, minor, patch;
    //      read_version_info("version.ini", major, minor, patch);
    //      std::cout << "Version: " << major << "." << minor << "." << patch << std::endl;
    std::string get_file_md5(const std::string& file_path) {
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
        return ss.str();
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