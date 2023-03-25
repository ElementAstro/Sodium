/*
 * ssh.cpp
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

#include "ssh.hpp"

#include <spdlog/spdlog.h>

    SSHClient::SSHClient(const std::string& ip, const std::string& username, const std::string& password, int port) {
        m_ip = ip;
        m_username = username;
        m_password = password;
        m_port = port;
        m_connected = false;
    }

    SSHClient::~SSHClient() {
        if (m_connected) {
            ssh_disconnect(m_session);
            ssh_free(m_session);
        }

        // 关闭日志
        spdlog::drop_all();
    }

    bool SSHClient::connect() {
        // 建立SSH连接
        m_session = ssh_new();
        if (!m_session) {
            spdlog::error("Failed to create SSH session");
            return false;
        }

        ssh_options_set(m_session, SSH_OPTIONS_HOST, m_ip.c_str());
        ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);
        ssh_options_set(m_session, SSH_OPTIONS_USER, m_username.c_str());

        int rc = ssh_connect(m_session);
        if (rc != SSH_OK) {
            spdlog::error("Failed to connect to SSH server: {}", ssh_get_error(m_session));
            ssh_free(m_session);
            return false;
        }

        rc = ssh_userauth_password(m_session, NULL, m_password.c_str());
        if (rc != SSH_AUTH_SUCCESS) {
            spdlog::error("Failed to authenticate: {}", ssh_get_error(m_session));
            ssh_disconnect(m_session);
            ssh_free(m_session);
            return false;
        }

        // 建立SFTP连接
        m_sftp = sftp_new(m_session);
        if (!m_sftp) {
            spdlog::error("Failed to create SFTP session");
            ssh_disconnect(m_session);
            ssh_free(m_session);
            return false;
        }

        rc = sftp_init(m_sftp);
        if (rc != SSH_OK) {
            spdlog::error("Failed to initialize SFTP session: {}", ssh_get_error(m_session));
            sftp_free(m_sftp);
            ssh_disconnect(m_session);
            ssh_free(m_session);
            return false;
        }

        m_connected = true;
        spdlog::info("Connected to SSH server");
        return true;
    }

    bool SSHClient::disconnect() {
        if (m_connected) {
            ssh_disconnect(m_session);
            ssh_free(m_session);
        }

        // 关闭日志
        spdlog::drop_all();
        return true;
    }

    bool SSHClient::execCommand(const std::string& command, std::string& output) {
        if (!m_connected) {
            spdlog::error("Not connected to SSH server");
            return false;
        }

        ssh_channel channel = ssh_channel_new(m_session);
        if (!channel) {
            spdlog::error("Failed to create SSH channel: {}", ssh_get_error(m_session));
            return false;
        }

        int rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
            spdlog::error("Failed to open SSH channel: {}", ssh_get_error(m_session));
            ssh_channel_free(channel);
            return false;
        }

        rc = ssh_channel_request_exec(channel, command.c_str());
        if (rc != SSH_OK) {
            spdlog::error("Failed to execute command '{}': {}", command, ssh_get_error(m_session));
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return false;
        }

        char buffer[1024];
        int nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        while (nbytes > 0) {
            output.append(buffer, nbytes);
            nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        }

        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);

        return true;
    }

    bool SSHClient::uploadFile(const std::string& localPath, const std::string& remotePath) {
        if (!m_connected) {
            spdlog::error("Not connected to SSH server");
            return false;
        }

        sftp_file file = sftp_open(m_sftp, remotePath.c_str(), O_WRONLY | O_CREAT, S_IRWXU);
        if (!file) {
            spdlog::error("Failed to open remote file '{}': {}", remotePath, ssh_get_error(m_session));
            return false;
        }

        FILE* fp = fopen(localPath.c_str(), "rb");
        if (!fp) {
            spdlog::error("Failed to open local file '{}'", localPath);
            sftp_close(file);
            return false;
        }

        char buffer[1024];
        size_t nread;
        while ((nread = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (sftp_write(file, buffer, nread) != nread) {
                spdlog::error("Failed to write to remote file '{}': {}", remotePath, ssh_get_error(m_session));
                fclose(fp);
                sftp_close(file);
                return false;
            }
        }

        fclose(fp);
        sftp_close(file);

        return true;
    }

    bool SSHClient::downloadFile(const std::string& remotePath, const std::string& localPath) {
        if (!m_connected) {
            spdlog::error("Not connected to SSH server");
            return false;
        }

        sftp_file file = sftp_open(m_sftp, remotePath.c_str(), O_RDONLY, S_IRWXU);
        if (!file) {
            spdlog::error("Failed to open remote file '{}': {}", remotePath, ssh_get_error(m_session));
            return false;
        }

        FILE* fp = fopen(localPath.c_str(), "wb");
        if (!fp) {
            spdlog::error("Failed to create local file '{}'", localPath);
            sftp_close(file);
            return false;
        }

        char buffer[1024];
        ssize_t nread;
        while ((nread = sftp_read(file, buffer, sizeof(buffer))) > 0) {
            if (fwrite(buffer, 1, nread, fp) != (size_t)nread) {
                spdlog::error("Failed to write to local file '{}'", localPath);
                fclose(fp);
                sftp_close(file);
                return false;
            }
        }

        fclose(fp);
        sftp_close(file);

        return true;
    }