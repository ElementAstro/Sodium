/*
 * event_server_ws.cpp
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
 
Date: 2023-3-14
 
Description: Main websocket server
 
**************************************************/

#include "event_server_ws.h"

#include <spdlog/spdlog.h>

#include <fstream>

typedef std::uint64_t hash_t;
constexpr hash_t prime = 0x100000001B3ull;  
constexpr hash_t basis = 0xCBF29CE484222325ull;  

namespace LightGuider
{

    Event_WS_Server Evt_WS_Server;

    Event_WS_Server::Event_WS_Server()
    {
        /*初始化WebSocket服务器*/
        /*加载设置*/
        m_server.clear_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
        /*初始化服务器*/
        m_server.init_asio();
        /*设置重新使用端口*/
        m_server.set_reuse_addr(true);
        /*设置打开事件*/
        m_server.set_open_handler(bind(&Event_WS_Server::on_open, this , ::_1));
        /*设置关闭事件*/
        m_server.set_close_handler(bind(&Event_WS_Server::on_close, this , ::_1));
        /*设置事件*/
        m_server.set_message_handler(bind(&Event_WS_Server::on_message, this ,::_1,::_2));
    }

    Event_WS_Server::~Event_WS_Server()
    {
        spdlog::info("");
    }

    void Event_WS_Server::on_open(websocketpp::connection_hdl hdl)
    {
        SaveClientInfo(hdl);
        if(Ws_Info->ClientCount + 1 > Ws_Info->MaxClientCount)
        {
            spdlog::warn("Too many client!");
            /*need more*/
        }
        else
        {
            Ws_Info->ClientCount ++;
            m_connections.insert(hdl);
        }
    }

    bool Event_WS_Server::SaveClientInfo(websocketpp::connection_hdl hdl)
    {
        light_ws_server::connection_ptr con = m_server.get_con_from_hdl( hdl );
        std::string name = "client.json";
        configor::json::value j;
        j["Host"] = con->get_host();
        j["Port"] = con->get_port();
        j["MaxHttpSize"] = con->get_max_http_body_size();
        j["MaxMsgSize"] = con->get_max_message_size();
        j["Path"] = con->get_resource();
        std::fstream tmpi(name,std::ios::in | std::ios::trunc);
        if(!tmpi.is_open())
            spdlog::error("Couldn't not save client infomation");
        else
            spdlog::info("Save client information in file");
        tmpi.close();
        return true;
    }

    /*
     * name: on_close(websocketpp::connection_hdl hdl)
     * @param hdl:WebSocket句柄
     * describe: Clear data on server disconnection
     * 描述：服务器断开连接时清空数据
     */
    void Event_WS_Server::on_close(websocketpp::connection_hdl hdl)
    {
        spdlog::info("Disconnected client.Goodbye!");
        m_connections.erase(hdl);
        Ws_Info->ClientCount --;
    }

        /*
     * name: on_message(websocketpp::connection_hdl hdl,message_ptr msg)
     * @param hdl:WebSocket句柄
     * @param msg：服务器信息
     * describe: Processing information from clients
     * 描述：处理来自客户端的信息
     * calls: readJson(std::string message)
     */
    void Event_WS_Server::on_message(websocketpp::connection_hdl hdl,message_ptr msg)
    {
        JsonParser(msg->get_payload());
    }

    /*以下三个函数均是用于switch支持string*/
	constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)  
    {  
        return *str ? hash_compile_time(str+1, (*str ^ last_value) * prime) : last_value;  
    }  

    hash_t hash_(char const* str)  
    {  
        hash_t ret{basis};  
        while(*str){  
            ret ^= *str;  
            ret *= prime;  
            str++;  
        }  
        return ret;  
    }  
    
    constexpr unsigned long long operator "" _hash(char const* p, size_t)
    {
        return hash_compile_time(p);
    }

    void Event_WS_Server::JsonParser(std::string message)
    {
        configor::json::value j = configor::json::parse(message);

    }

    /*
     * name: send(std::string payload)
     * @param message:需要发送的信息
     * describe: Send information to client both ws and wss
     * 描述：向ws和wss客户端发送信息
     * note: The message must be sent in the format of JSON
     */
    void Event_WS_Server::send(std::string message)
    {
        if(Ws_Info->State == CONNECTED)
        {
            /*向WS客户端发送信息*/
            for (auto it : m_connections)
            {
                try
                {
                    m_server.send(it, message, websocketpp::frame::opcode::text);
                }
                catch (websocketpp::exception const &e)
                {
                    std::cerr << e.what() << std::endl;
                }
                catch (...)
                {
                    std::cerr << gettext("other exception") << std::endl;
                }
            }
        }
    }

    /*
     * name: stop()
     * describe: Stop the websocket server
     * 描述：停止WebSocket服务器
     */
    void Event_WS_Server::stop()
    {
        for (auto it : m_connections)
        {
            m_server.close(it, websocketpp::close::status::normal, gettext("Switched off by user."));
        }
        spdlog::info("Stop server.Goodbye!");
        /*清除服务器句柄*/
        m_connections.clear();
        /*停止服务器*/
        m_server.stop();
    }

    /*
     * name: is_running()
     * @return Boolean function:服务器运行状态
     *  -false: server is not running||服务器不在运行
     *  -true: server is running||服务器正在运行
     * note: This function seems useless,Maybe you will never use it
     */
    bool Event_WS_Server::is_running()
    {
        if(m_server.is_listening())
            spdlog::info("Server is running properly");
        else
            spdlog::error("Server isn't running ,please check!");
        return m_server.is_listening();
    }
    
    /*
     * name: run(int port)
     * @param port:服务器端口
     * describe: This is used to start the websocket server
     * 描述：启动WebSocket服务器
	 * calls: IDLog(const char *fmt, ...)
     */
    void Event_WS_Server::run(int port)
    {
        try
        {
            spdlog::info("Websocket server start at {} and keep listening",port);
            /*设置端口为IPv4模式并指定端口*/
            m_server.listen(websocketpp::lib::asio::ip::tcp::v4(),port);
            m_server.set_reuse_addr(true);
            m_server.start_accept();
            m_server.run();
        }
        catch (websocketpp::exception const & e)
        {   
			std::cerr << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << gettext("other exception") << std::endl;
        }
    }
}