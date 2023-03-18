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

namespace fs = std::filesystem;

namespace LightGuider{
    
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
}