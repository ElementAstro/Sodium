/*
 * app.cpp
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
 
Date: 2023-3-5
 
Description: Main web server
 
**************************************************/

#include "app.h"
#include "url.h"
#include "ws.h"

#include <boost/lexical_cast.hpp>

using namespace crow::utility;

#include <sstream>

crow::SimpleApp app;

void run_http_server()
{
        load_url();
        // enables all log
        app.loglevel(crow::LogLevel::DEBUG);
        //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

        app.port(5000)
            .server_name("LightServer")
            .timeout(100)
            .multithreaded()
            .run();
}


int main(){
    run_http_server();
    return 0;
}
