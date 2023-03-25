/*
 * ssh.hpp
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
 
Date: 2023-3-25
 
Description: SSH Client
 
**************************************************/

#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <libssh/ssh2.h>

#include <string>

class SSHClient {
    public:
        SSHClient(const std::string& ip, const std::string& username, const std::string& password, int port = 22);
        ~SSHClient();
        bool connect();
        bool disconnect();
        bool execCommand(const std::string& command, std::string& output);
        bool uploadFile(const std::string& localPath, const std::string& remotePath);
        bool downloadFile(const std::string& remotePath, const std::string& localPath);
    private:
        std::string m_ip;
        std::string m_username;
        std::string m_password;
        int m_port;
        ssh_session m_session;
        sftp_session m_sftp;
        bool m_connected;
};
