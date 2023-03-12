#include "crow.h"
#include <boost/lexical_cast.hpp>

using namespace crow::utility;

#include <sstream>

class ExampleLogHandler : public crow::ILogHandler {
    public:
        void log(std::string /*message*/, crow::LogLevel /*level*/) override {
//            cerr << "ExampleLogHandler -> " << message;
        }
};

struct ExampleMiddleware 
{
    std::string message;

    ExampleMiddleware() 
    {
        message = "foo";
    }

    void setMessage(std::string newMsg)
    {
        message = newMsg;
    }

    struct context
    {
    };

    void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/)
    {
        // no-op
    }
};

int main()
{
    crow::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    CROW_ROUTE(app, "/")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("index.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/bugreport")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("bugreport.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/client")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("client.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/debug")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("debug.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/device")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("device.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/error")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("error.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/faq")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("faq.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/forget-password")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("device.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/idebug")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("idebug.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/license")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("license.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/lockscreen")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("lockscreen.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/ndesktop")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("ndesktop.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/novnc")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("novnc.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/register")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("register.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/scripteditor")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("scripteditor.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/skymap")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("skymap.html");
        return page.render(x);
    });

    CROW_ROUTE(app, "/webssh")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
        auto page = crow::mustache::load("webssh.html");
        return page.render(x);
    });

    // enables all log
    app.loglevel(crow::LogLevel::DEBUG);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .multithreaded()
        .run();
}
