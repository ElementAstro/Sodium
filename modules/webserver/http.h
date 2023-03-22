#ifndef __HTTP_H__
#define __HTTP_H__

#include "app.h"

void load_http(){
    CROW_ROUTE(app, "/hello/<string>")
    ([](const crow::request& req, crow::response& res, std::string name) {
        res.set_header("Content-Type", "text/plain");
        res.write("Hello " + name + "!\n");
        res.end();
    });

    // 处理HTTP POST请求
    CROW_ROUTE(app, "/submit")
    ([](const crow::request& req, crow::response& res) {
        auto x = crow::json::load(req.body);
        if (!x) {
            res.code = 400;
            res.write("Bad Request");
            res.end();
            return;
        }
        std::string name = x["name"].s();
        std::string email = x["email"].s();
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue result;
        result["code"] = 0;
        result["message"] = "Success";
        result["data"]["name"] = name;
        result["data"]["email"] = email;
        res.end();
    });
}

#endif