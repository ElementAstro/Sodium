#ifndef __URL_H__
#define __URL_H__

#include "app.h"

/// @brief Load all of the URL will be able to enter by clients
void load_url(){
    CROW_ROUTE(app, "/")
    ([]{
        auto page = crow::mustache::load("index.html");
        return page.render();
    });

    CROW_ROUTE(app, "/bugreport")
    ([]{
        auto page = crow::mustache::load("bugreport.html");
        return page.render();
    });

    CROW_ROUTE(app, "/client")
    ([]{
        auto page = crow::mustache::load("client.html");
        return page.render();
    });

    CROW_ROUTE(app, "/debug")
    ([]{
        auto page = crow::mustache::load("debug.html");
        return page.render();
    });

    CROW_ROUTE(app, "/device")
    ([]{
        auto page = crow::mustache::load("device.html");
        return page.render();
    });

    CROW_ROUTE(app, "/error")
    ([]{
        auto page = crow::mustache::load("error.html");
        return page.render();
    });

    CROW_ROUTE(app, "/faq")
    ([]{
        auto page = crow::mustache::load("faq.html");
        return page.render();
    });

    CROW_ROUTE(app, "/forget-password")
    ([]{
        auto page = crow::mustache::load("device.html");
        return page.render();
    });

    CROW_ROUTE(app, "/idebug")
    ([]{
        auto page = crow::mustache::load("idebug.html");
        return page.render();
    });

    CROW_ROUTE(app, "/license")
    ([]{
        auto page = crow::mustache::load("license.html");
        return page.render();
    });

    CROW_ROUTE(app, "/lockscreen")
    ([]{
        auto page = crow::mustache::load("lockscreen.html");
        return page.render();
    });

    CROW_ROUTE(app, "/ndesktop")
    ([]{
        auto page = crow::mustache::load("ndesktop.html");
        return page.render();
    });

    CROW_ROUTE(app, "/ndesktop-browser")
    ([]{
        auto page = crow::mustache::load("ndesktop-browser.html");
        return page.render();
    });

    CROW_ROUTE(app, "/ndesktop-store")
    ([]{
        auto page = crow::mustache::load("ndesktop-store.html");
        return page.render();
    });

    CROW_ROUTE(app, "/ndesktop-system")
    ([]{
        auto page = crow::mustache::load("ndesktop-system.html");
        return page.render();
    });

    CROW_ROUTE(app, "/novnc")
    ([]{
        auto page = crow::mustache::load("novnc.html");
        return page.render();
    });

    CROW_ROUTE(app, "/register")
    ([]{
        auto page = crow::mustache::load("register.html");
        return page.render();
    });

    CROW_ROUTE(app, "/scripteditor")
    ([]{
        auto page = crow::mustache::load("scripteditor.html");
        return page.render();
    });

    CROW_ROUTE(app, "/skymap")
    ([]{
        auto page = crow::mustache::load("skymap.html");
        return page.render();
    });

    CROW_ROUTE(app, "/system")
    ([]{
        auto page = crow::mustache::load("system.html");
        return page.render();
    });

    CROW_ROUTE(app, "/test")
    ([]{
        auto page = crow::mustache::load("test.html");
        return page.render();
    });

    CROW_ROUTE(app, "/webssh")
    ([]{
        auto page = crow::mustache::load("webssh.html");
        return page.render();
    });

    CROW_ROUTE(app, "/json/particles.json%22")
    ([](const crow::request&, crow::response& res) {
        //replace cat.jpg with your file path
        res.set_static_file_info("static/json/particles.json");
        res.end();
    });
}


#endif