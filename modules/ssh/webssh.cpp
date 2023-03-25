// main.cpp
#include "crow.h"
#include <iostream>
#include "ssh.hpp"

using namespace std;

int main()
{
    crow::SimpleApp app;
    SSHClient ssh("", "", "");  // 初始化一个SSHClient对象

    // 前端页面
    CROW_ROUTE(app, "/")([](){
        return crow::mustache::load("index.html").render();
    });

    // WebSocket 连接路由
    CROW_WEBSOCKET_ROUTE(app, "/wwebssh")
    .onopen([&ssh](crow::websocket::connection& conn){
        cout << "opened" << endl;  // 输出信息供调试
        conn.send_text("Welcome to WebSSH!");  // 初次连接发送欢迎消息
    })
    .onclose([](crow::websocket::connection& conn, const string& reason){
        cout << "closed: " << reason << endl;  // 输出信息供调试
    })
    .onmessage([&ssh](crow::websocket::connection& conn, const string& data, bool is_binary){
        if(data.empty()){  // 判断输入命令是否为空
            conn.send_text("Error: Empty command!");
            return;
        }
        try{
            cout << data << endl;  // 输出信息供调试
            string output;
            ssh.connect();  // 建立SSH连接
            ssh.execCommand(data, output);  // 执行指定的命令
            ssh.disconnect();  // 断开SSH连接
            conn.send_text(output);  // 将输出结果发送给前端页面
        }
        catch(const exception& e){  // 捕获可能抛出的异常
            conn.send_text("Error: " + string(e.what()));
        }
    });

    app.port(18080).multithreaded().run();  // 启动服务器

    return 0;
}
