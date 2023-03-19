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

#include "lua/lua.hpp"
#include <Python.h>

#include <vector>
#include <unordered_map>

#include <spdlog/spdlog.h>

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
            bool LoadPythonScript(const std::string& scriptName);
            void UnloadPythonScript(const std::string& scriptName);

            template<typename T>
            T GetFunction(const std::string& module_name, const std::string& function_name) {
                // 获取动态库句柄
                auto handle = handles_[module_name];
                // 获取函数指针
                void* func_ptr = dlsym(handle, function_name.c_str());
                if (!func_ptr) {
                    spdlog::error("Failed to get symbol {} from module {}", function_name, module_name);
                    return nullptr; // 函数不存在，返回空指针
                }
                // 转换函数指针类型，并返回该函数指针
                return reinterpret_cast<T>(func_ptr);
            }

            std::vector<std::string> getPythonFunctions(const std::string& scriptName);

            /// @brief 从已加载的模块中加载函数并运行
            /// @tparam T 函数类型
            /// @tparam ...Args 函数所需参数 
            /// @tparam class_type 类类型
            /// @param module_name 模块名称
            /// @param func_name 函数名称
            /// @param thread_name 线程名称
            /// @param instance 类实例
            /// @param ...args 
            /// @return 函数是否启动成功
            template<typename T, typename class_type, typename... Args>
            typename std::enable_if<std::is_class<class_type>::value, bool>::type
            LoadAndRunFunction(const std::string& module_name, const std::string& func_name,
                                const std::string& thread_name, class_type* instance, Args... args) {
                typedef T (class_type::*MemberFunctionPtr)(Args...);
                typedef T (*FunctionPtr)(Args...);
                void* handle = GetHandle(module_name);
                auto sym_ptr = dlsym(handle, func_name.c_str());
                if (!sym_ptr) {
                    spdlog::error("Failed to load symbol {}: {}", func_name, dlerror());
                    return false;
                }
                FunctionPtr func_ptr = reinterpret_cast<FunctionPtr>(sym_ptr);
                MemberFunctionPtr member_func_ptr = nullptr;
                if constexpr (std::is_member_function_pointer_v<MemberFunctionPtr>) {
                    member_func_ptr = reinterpret_cast<MemberFunctionPtr>(sym_ptr);
                }
                ThreadManage.addThread(std::bind(member_func_ptr, instance, args...), std::bind(func_ptr, args...), thread_name);
                return true;
            }

            template<typename T, typename... Args>
            T LoadAndRunFunction(const std::string& module_name, const std::string& func_name,
                                const std::string& thread_name, Args&&... args) {
                typedef T (*FunctionPtr)(Args...);
                void* handle = GetHandle(module_name);
                auto sym_ptr = dlsym(handle, func_name.c_str());
                if (!sym_ptr) {
                    spdlog::error("Failed to load symbol {}: {}", func_name, dlerror());
                }
                FunctionPtr func_ptr = reinterpret_cast<FunctionPtr>(sym_ptr);
                ThreadManage.addThread(std::bind(func_ptr, std::forward<Args>(args)...), thread_name);
            }


            std::string LoadAndRunLuaFunction(const std::string& funcName);

            template<typename... Args>
            bool runPythonFunction(const std::string& scriptName, const std::string& functionName, Args... args) {
                auto iter = python_modules_.find(scriptName);
                if (iter == python_modules_.end()) {
                    spdlog::error("Script not found: {}",scriptName);
                    return false;
                }

                PyObject* pModule = iter->second;
                PyObject* pFunc = PyObject_GetAttrString(pModule, functionName.c_str());
                if (!pFunc || !PyCallable_Check(pFunc)) {
                    spdlog::error("Function not found: {}",functionName);
                    Py_XDECREF(pFunc);
                    return false;
                }

                PyObject* pArgs = PyTuple_New(sizeof...(args));
                int i = 0;
                (void)std::initializer_list<int>{(PyTuple_SetItem(pArgs, i++, Py_BuildValue("d", args)), 0)...};

                PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
                if (!pResult) {
                    PyErr_Print();
                    Py_XDECREF(pFunc);
                    Py_XDECREF(pArgs);
                    return false;
                }

                Py_XDECREF(pFunc);
                Py_XDECREF(pArgs);
                Py_XDECREF(pResult);

                return true;
            }

            /*
            runFunction("my_script.py", "myFunction", scriptLoader, [](){
                std::cout << "myFunction has finished executing." << std::endl;
            }, 42, "Hello, world!");
            */
            template<typename F, typename... Args>
            void AysncRunPythonFunction(const std::string& scriptName, const std::string& functionName, ModuleLoader& scriptLoader, F&& callback, Args&&... args) {
                auto task = [scriptName, functionName, &scriptLoader, args..., callback]() {
                    PyGILState_STATE gstate = PyGILState_Ensure();

                    scriptLoader.AysncRunPythonFunction(scriptName, functionName, std::forward<Args>(args)...);

                    PyGILState_Release(gstate);

                    if (callback) {
                        callback();
                    }
                };

                if (callback) {
                    std::async(std::launch::async, std::move(task));
                } else {
                    task();
                }
            }

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

            std::unordered_map<std::string, PyObject*> python_modules_;
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