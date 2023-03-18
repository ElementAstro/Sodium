/*
 * modloader.h
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

#ifndef __MOD_LOADER_H__
#define __MOD_LOADER_H__

#include <lua5.4/lua.hpp>

#include <vector>
#include <unordered_map>

#include "chaiscript/chaiscript.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"

namespace LightGuider{

    configor::json::value iterator_modules_dir();
    
    class ModuleLoader {
        public:
            ModuleLoader();
            ~ModuleLoader();
            bool LoadModule(const std::string& path, const std::string& name);
            bool loadLuaModule(const std::string& moduleName, const std::string& fileName);
            bool LoadChaiScript(const std::string& filename);

        public:
            void* GetHandle(const std::string& name) const {
                auto it = handles_.find(name);
                if (it == handles_.end()) {
                    return nullptr;
                }
                return it->second;
            }

            lua_State* getLuaModule(const std::string& moduleName) const {
                auto it = lua_modules_.find(moduleName);
                if (it != lua_modules_.end()) {
                    return L_;
                }
                return nullptr;
            }

            const std::vector<std::string>& getFunctionList(const std::string& moduleName) const {
                auto it = lua_modules_.find(moduleName);
                if (it != lua_modules_.end()) {
                    return it->second;
                }
                static const std::vector<std::string> emptyList;
                return emptyList;
            }

        protected:

            // 获取函数和变量列表
            const std::unordered_map<std::string, std::variant<chaiscript::Type_Info, chaiscript::Boxed_Value>>& get_functions_and_variables() const {
                return chai_functions_;
            }
            
        private:
            std::unordered_map<std::string, void*> handles_;
            std::unordered_map<std::string, std::vector<std::string>> lua_modules_;
            lua_State* L_;

            std::unique_ptr<chaiscript::ChaiScript> chai_modules_;
            // chai_functions_用于存储函数和变量列表
            std::unordered_map<std::string, std::variant<chaiscript::Type_Info, chaiscript::Boxed_Value>> chai_functions_;
    };

    extern ModuleLoader module_loader;

}

#endif

/*
Example:

    loader.loadLuaModule("module1", "module1.lua");
    loader.loadLuaModule("module2", "module2.lua");
    const auto& functionList1 = loader.getFunctionList("module1");
    const auto& functionList2 = loader.getFunctionList("module2");
    std::cout << "Functions in module1:" << std::endl;
    for (const auto& functionName : functionList1) {
        std::cout << "  " << functionName << std::endl;
    }
    std::cout << "Functions in module2:" << std::endl;
    for (const auto& functionName : functionList2) {
        std::cout << "  " << functionName << std::endl;
    }

    chai

    if (loader.load_script("myscript.chai")) {
        // 获取函数和变量列表
        const std::unordered_map<std::string, std::variant<chaiscript::Type_Info, chaiscript::Boxed_Value>>& chai_functions_ = loader.get_functions_and_variables();
        // 遍历函数和变量列表，输出每个函数和变量的名称和类型
        for (const auto& item : chai_functions_) {
            std::ostringstream oss;
            oss << "Name: " << item.first << ", Type: ";
            if (item.second.index() == 0) {
                chaiscript::Type_Info info = std::get<0>(item.second);
                oss << "Function, Return Type: " << info.bare_name();
            } else if (item.second.index() == 1) {
                chaiscript::Boxed_Value value = std::get<1>(item.second);
                oss << "Variable, Type: " << value.get_type_info().bare_name();
            }
            logger->info(oss.str());
        }
    }
*/