/*
 * event_server_ws.h
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
 
Description: Main websocket server head file
 
**************************************************/

#ifndef _EVENT_WS_SERVER_H_
#define _EVENT_WS_SERVER_H_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "configor/json.hpp"

#include <set>

typedef websocketpp::server<websocketpp::config::asio> light_ws_server;
typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> con_list;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::condition_variable;
typedef light_ws_server::message_ptr message_ptr;

typedef enum { IDEL,CONNECTED,CONNECTING,STOPPED } ServerState;


namespace LightGuider
{
    class Event_WS_Server
    {
        public:
            explicit Event_WS_Server();
            ~Event_WS_Server();

            void run(int port);
        
        protected:
            virtual void on_open(websocketpp::connection_hdl hdl);
			virtual void on_close(websocketpp::connection_hdl hdl);
			virtual void on_message(websocketpp::connection_hdl hdl,message_ptr msg);
            virtual void send(std::string message);
            virtual void stop();
            virtual bool is_running();

            virtual bool SaveClientInfo(websocketpp::connection_hdl hdl);
            virtual void JsonParser(std::string message);
        
        private:
            light_ws_server m_server;
			con_list m_connections;

            struct WsInfo
            {
                ServerState State = IDEL;
                std::atomic_int ClientCount;
                std::atomic_int MaxClientCount;
                std::atomic_int Timeout;
                std::atomic_int MaxThreadCount;
            }*Ws_Info;
    };
    extern Event_WS_Server Evt_WS_Server;
}

#endif