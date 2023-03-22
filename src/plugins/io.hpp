#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <openssl/md5.h>
#include "spdlog/spdlog.h"

class FileManager {
    public:
        FileManager() = default;
        ~FileManager() = default;

        /**
         * @brief 创建指定的文件。
         * 如果文件已经存在，将返回false，并打印错误信息。
         * 在创建文件后，我们还将使用标准C库中的std::fopen()函数来创建一个链接到该文件，
         * 这样可以通过多个名称访问该文件。
         * @param filename 文件名
         * @return 是否成功创建文件
         */
        bool createFile(const std::string& filename);

        /**
         * @brief 打开指定的文件
         * 如果文件不存在，将返回false，并打印错误信息。
         * @param filename 文件名
         * @return 是否成功打开文件
         */
        bool openFile(const std::string& filename);

        /**
         * @brief 读取当前打开的文件的内容
         * 如果当前没有打开的文件，将返回false，并打印错误信息。
         * @param contents 用于存储读取的内容的字符串
         * @return 是否成功读取文件
         */
        bool readFile(std::string& contents);

        /**
         * @brief 写当前打开的文件
         * 如果当前没有打开的文件，将返回false，并打印错误信息。
         * @param contents 要写入文件的内容字符串
         * @return 是否成功写文件
         */
        bool writeFile(const std::string& contents);

        /**
         * @brief 将旧文件重命名为新文件
         * 如果旧文件不存在或新文件已经存在，将返回false，并打印错误信息。
         * @param oldFilename 旧文件名
         * @param newFilename 新文件名
         * @return 是否成功移动文件
         */
        bool moveFile(const std::string& oldFilename, const std::string& newFilename);

        /**
         * @brief 删除指定的文件
         * 如果文件不存在，将返回false，并打印错误信息。
         * @param filename 文件名
         * @return 是否成功删除文件
         */
        bool deleteFile(const std::string& filename);

        /**
         * @brief 获取当前打开的文件的大小（单位：字节）
         * 如果文件大小获取失败，将返回-1。
         * 如果当前没有打开的文件，将返回false，并打印错误信息。
         * @return 文件大小（单位：字节）
         */
        long getFileSize();

        /**
         * @brief 计算当前打开的文件的MD5值
         * 如果当前没有打开的文件，将返回空字符串，并打印错误信息。
         * @return 文件的MD5值
         */
        std::string calculateMD5();

        /**
         * @brief 获取指定文件所在目录的路径
         * 如果文件不存在或者发生其他错误，将返回空字符串，并打印错误信息。
         * @param filename 文件名
         * @return 文件所在目录的路径
         */
        std::string getFileDirectory(const std::string& filename);

    private:
        std::string m_filename; ///< 当前打开的文件名
        std::fstream m_file; ///< 当前打开的文件流
};

/**
 * @brief 检查文件是否存在。
 * @param filename 文件名
 * @return 文件是否存在
 */
bool fileExists(const std::string& filename) {
    std::ifstream infile(filename.c_str());
    return infile.good();
}

bool FileManager::createFile(const std::string& filename) {
    if (fileExists(filename)) {
        spdlog::error("File \"{}\" already exists!", filename);
        return false;
    }
    std::ofstream outfile(filename.c_str());
    if (!outfile) {
        spdlog::error("Error creating file \"{}\"!", filename);
        return false;
    }
    outfile.close();
    std::fclose(std::fopen(filename.c_str(), "a")); // create a link to the file
    spdlog::info("Created file \"{}\"", filename);
    return true;
}

bool FileManager::openFile(const std::string& filename) {
    if (!fileExists(filename)) {
        spdlog::error("File \"{}\" does not exist!", filename);
        return false;
    }
    m_filename = filename;
    m_file.open(filename.c_str(), std::ios::in | std::ios::out);
    if (!m_file) {
        spdlog::error("Could not open file \"{}\"!", filename);
        return false;
    }
    spdlog::info("Opened file \"{}\"", filename);
    return true;
}

bool FileManager::readFile(std::string& contents) {
    if (!m_file.is_open()) {
        spdlog::error("No file is currently open!");
        return false;
    }
    std::stringstream buffer;
    buffer << m_file.rdbuf();
    contents = buffer.str();
    spdlog::info("Read contents of file \"{}\"", m_filename);
    return true;
}

bool FileManager::writeFile(const std::string& contents) {
    if (!m_file.is_open()) {
        spdlog::error("No file is currently open!");
        return false;
    }
    m_file << contents;
    spdlog::info("Wrote contents to file \"{}\"", m_filename);
    return true;
}

bool FileManager::moveFile(const std::string& oldFilename, const std::string& newFilename) {
    if (!fileExists(oldFilename)) {
        spdlog::error("File \"{}\" does not exist!", oldFilename);
        return false;
    }
    if (fileExists(newFilename)) {
        spdlog::error("File \"{}\" already exists!", newFilename);
        return false;
    }
    int result = std::rename(oldFilename.c_str(), newFilename.c_str());
    if (result != 0) {
        spdlog::error("Could not move file \"{}\" to \"{}\"!", oldFilename, newFilename);
        return false;
    }
    spdlog::info("Moved file from \"{}\" to \"{}\"", oldFilename, newFilename);
    return true;
}

bool FileManager::deleteFile(const std::string& filename) {
    if (!fileExists(filename)) {
        spdlog::error("File \"{}\" does not exist!", filename);
        return false;
    }
    if (remove(filename.c_str()) != 0) {
        spdlog::error("Could not delete file \"{}\"!", filename);
        return false;
    }
    spdlog::info("Deleted file \"{}\"", filename);
    return true;
}

long FileManager::getFileSize() {
    if (!m_file.is_open()) {
        spdlog::error("No file is currently open!");
        return -1;
    }
    m_file.seekg(0, m_file.end);
    long fileSize = m_file.tellg();
    m_file.seekg(0, m_file.beg);
    if (fileSize == -1) {
        spdlog::error("Could not get file size of \"{}\"!", m_filename);
    } else {
        spdlog::info("File size of \"{}\" is {} bytes", m_filename, fileSize);
    }
    return fileSize;
}

std::string FileManager::calculateMD5() {
    if (!m_file.is_open()) {
        spdlog::error("No file is currently open!");
        return "";
    }
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    char buffer[1024];
    while (m_file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5Context, buffer, sizeof(buffer));
    }
    MD5_Final(reinterpret_cast<unsigned char*>(buffer), &md5Context);
    std::stringstream md5Stream;
    md5Stream << std::hex << std::setfill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        md5Stream << std::setw(2) << static_cast<int>(buffer[i]);
    }
    spdlog::info("MD5 value for file \"{}\" is {}", m_filename, md5Stream.str());
    return md5Stream.str();
}

std::string FileManager::getFileDirectory(const std::string& filename) {
    size_t pos = filename.find_last_of("/\\");
    if (pos == std::string::npos) {
        spdlog::error("Could not get directory of file \"{}\"", filename);
        return "";
    } else {
        std::string directory = filename.substr(0, pos);
        spdlog::info("Directory of file \"{}\" is \"{}\"", filename, directory);
        return directory;
    }
}
