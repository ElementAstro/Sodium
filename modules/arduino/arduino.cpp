// 该程序旨在扫描串口以查找可用的Arduino板，使用向量容器存储可用端口并输出

// 包含必要的库
#include <iostream> // 标准输入输出流
#include <string>   // 字符串操作
#include <vector>   // 向量容器
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <spdlog/spdlog.h>

// 根据不同的操作系统选择不同的头文件和API
#ifdef _WIN32
#include <windows.h> // Windows API
#else
#include <dirent.h>  // 目录操作相关API，Linux/Mac OS通用
#include <fcntl.h>   // 文件操作相关API，Linux/Mac OS通用
#endif

using namespace std;

// 扫描串口并返回可用端口的函数
vector<string> scanPorts() {
    vector<string> portList; // 用于存储可用端口的字符串型向量容器

    // 根据不同操作系统选择不同的串口搜索方式
    #ifdef _WIN32
        // 在Windows中搜索所有串口
        char *portName = "\\\\.\\COM"; // 串口名称前缀
        DWORD numPorts = 255;           // 最大串口号
        for (DWORD i = 1; i <= numPorts; i++) { // 遍历所有串口
            string fullPortName = string(portName) + to_string(i); // 构造完整串口名称
            HANDLE hSerial = CreateFileA(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // 使用Windows API打开当前串口
            if (hSerial != INVALID_HANDLE_VALUE) { // 判断是否可用
                CloseHandle(hSerial); // 关闭串口句柄
                DCB dcbSerialParams = {0}; // 初始化串口参数结构体
                dcbSerialParams.DCBlength = sizeof(dcbSerialParams); // 获取串口参数
                if (GetCommState(hSerial, &dcbSerialParams)) { // 判断是否是Arduino板
                    if (dcbSerialParams.BaudRate == CBR_115200 && dcbSerialParams.fParity == false &&
                        dcbSerialParams.ByteSize == 8 && dcbSerialParams.StopBits == ONESTOPBIT) {
                        portList.push_back(fullPortName); // 将可用串口添加到向量容器中
                    }
                }
            }
        }
    #else
        // 在Linux/Mac OS中搜索所有串口
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir("/dev")) != NULL) { // 打开/dev目录
            while ((ent = readdir(dir)) != NULL) { // 遍历/dev目录下的所有文件
                string devicePath = string("/dev/") + ent->d_name; // 构造当前设备路径名称
                int fd = open(devicePath.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK); // 使用Linux/Mac OS API打开当前串口
                if (fd != -1) { // 判断串口是否可用
                    close(fd); // 关闭串口
                    if (devicePath.find("ttyACM") != string::npos || devicePath.find("ttyUSB") != string::npos) { // 判断是否是Arduino板
                        int speed = B115200;
                        termios options;
                        memset(&options, 0, sizeof(options));
                        tcgetattr(fd, &options); // 获取串口参数
                        cfsetispeed(&options, speed);
                        cfsetospeed(&options, speed);
                        options.c_cflag |= (CLOCAL | CREAD);
                        options.c_cflag &= ~CSIZE;
                        options.c_cflag |= CS8;
                        options.c_cflag &= ~PARENB;
                        options.c_cflag &= ~CSTOPB;
                        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
                        options.c_oflag &= ~OPOST;
                        tcsetattr(fd, TCSANOW, &options); // 设置串口参数
                        char buf[256];
                        int n = write(fd, "testing\n", 8);
                        usleep(10000);
                        n = read(fd, buf, 256);
                        buf[n] = '\0';
                        if (strstr(buf, "Hello World!")) { // 判断当前端口是否可用
                            portList.push_back(devicePath); // 将可用串口添加到向量容器中
                        }
                    }
                }
            }
            closedir(dir);
        }
    #endif

    return portList; // 返回所有可用端口
}

class SerialPort {
  public:
    /**
     * @brief 构造函数，打开串口并设定波特率、数据位、停止位和奇偶校验位
     * @param portName 串口名称，例如"COM1"或"/dev/ttyS0"
     * @param baudRate 波特率，例如9600
     */
    SerialPort(const std::string& portName, int baudRate)
      : port_(io_service_), portName_(portName), baudRate_(baudRate) {
        try {
            port_.open(portName_);
            port_.set_option(boost::asio::serial_port_base::baud_rate(baudRate_));
            port_.set_option(boost::asio::serial_port_base::character_size(8));
            port_.set_option(boost::asio::serial_port_base::stop_bits(
                boost::asio::serial_port_base::stop_bits::one
            ));
            port_.set_option(boost::asio::serial_port_base::parity(
                boost::asio::serial_port_base::parity::none
            ));
            spdlog::info("Serial port {} opened with baud rate {}.", portName_, baudRate_);
        } catch (const std::exception& e) {
            spdlog::error("Failed to open serial port {}.\n{}", portName_, e.what());
            throw;
        }
    }

    /**
     * @brief 析构函数，关闭串口
     */
    ~SerialPort() {
        try {
            port_.close();
            spdlog::info("Serial port {} closed.", portName_);
        } catch (const std::exception& e) {
            spdlog::error("Failed to close serial port {}.\n{}", portName_, e.what());
        }
    }

    /**
     * @brief 从串口读取数据
     * @return 读取到的字符串，如果发生错误则返回空字符串
     */
    std::string readFromPort() {
        std::vector<char> buffer(1024);
        try {
            size_t bytesRead = boost::asio::read(port_, boost::asio::buffer(buffer));
            std::string output(buffer.begin(), buffer.begin() + bytesRead);
            spdlog::debug("Read {} bytes from serial port {}:\n{}", bytesRead, portName_, output);
            return output;
        } catch (const std::exception& e) {
            spdlog::error("Failed to read from serial port {}.\n{}", portName_, e.what());
            return "";
        }
    }

    /**
     * @brief 往串口写入数据
     * @param message 要写入的字符串
     * @return 写入成功返回true，否则返回false
     */
    bool writeToPort(const std::string& message) {
        try {
            boost::asio::write(port_, boost::asio::buffer(message));
            spdlog::debug("Wrote to serial port {}:\n{}", portName_, message);
            return true;
        } catch (const std::exception& e) {
            spdlog::error("Failed to write to serial port {}.\n{}", portName_, e.what());
            return false;
        }
    }

  private:
    boost::asio::io_service io_service_; // I/O服务类，用于执行异步操作，如串口读写 
    boost::asio::serial_port port_; // 串口类，用于读写串口数据 
    std::string portName_; // 串口名称，例如"COM1"或"/dev/ttyS0"
    int baudRate_; // 波特率，例如9600
};

using boost::asio::serial_port;
using boost::asio::io_service;

// 主函数
int main() {
    spdlog::info("Scanning available serial ports...");
    vector<string> ports = scanPorts();
    if (ports.empty()) {
        spdlog::error("No available Arduino boards found!");
        return 0;
    } else {
        spdlog::info("Available Arduino board(s) found on the following port(s):");
        for (auto port : ports) {
            spdlog::info(port);
        }
    }

    io_service ios;
    serial_port port(ios);

    std::string portName = "COM1";
    port.open(portName);
    port.set_option(boost::asio::serial_port_base::baud_rate(9600));
    port.set_option(boost::asio::serial_port_base::character_size(8));
    port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    std::string data;
    char buf[1024];

    auto read_serial = [&data, &port, &buf](){
        while (true) {
            size_t len = port.read_some(boost::asio::buffer(buf));
            for (size_t i = 0; i < len; i++) {
                char c = buf[i];
                if (c != '\r' && c != '\n') {
                    data += c;
                } else {
                    spdlog::info("Received data: {}", data);
                    data.clear();
                }
            }
        }
    };

    spdlog::info("Starting serial communication...");
    boost::asio::spawn(ios, [&](boost::asio::yield_context yield){
        read_serial();
    });

    ios.run();

    spdlog::info("Serial communication finished.");
}