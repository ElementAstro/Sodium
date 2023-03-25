#ifndef __URL_H__
#define __URL_H__

#include "app.h"

/// @brief Load all of the URL will be able to enter by clients
void load_url(){
    CROW_ROUTE(app, "/")
    ([]{
        return crow::mustache::load("index.html").render();
    });

    CROW_ROUTE(app, "/bugreport")
    ([]{
        return crow::mustache::load("bugreport.html").render();
    });

    CROW_ROUTE(app, "/client")
    ([]{
        return crow::mustache::load("client.html").render();
    });

    CROW_ROUTE(app, "/debug")
    ([]{
        return crow::mustache::load("debug.html").render();
    });

    CROW_ROUTE(app, "/device")
    ([]{
        return crow::mustache::load("device.html").render();
    });

    CROW_ROUTE(app, "/faq")
    ([]{
        return crow::mustache::load("faq.html").render();
    });

    CROW_ROUTE(app, "/forget-password")
    ([]{
        return crow::mustache::load("device.html").render();
    });

    CROW_ROUTE(app, "/idebug")
    ([]{
        return crow::mustache::load("idebug.html").render();
    });

    CROW_ROUTE(app, "/license")
    ([]{
        return crow::mustache::load("license.html").render();
    });

    CROW_ROUTE(app, "/lockscreen")
    ([]{
        return crow::mustache::load("lockscreen.html").render();
    });

    CROW_ROUTE(app, "/ndesktop")
    ([]{
        return crow::mustache::load_text("ndesktop.html");
    });

    CROW_ROUTE(app, "/ndesktop-browser")
    ([]{
        return crow::mustache::load_text("ndesktop-browser.html");
    });

    CROW_ROUTE(app, "/ndesktop-store")
    ([]{
        return crow::mustache::load_text("ndesktop-store.html");
    });

    CROW_ROUTE(app, "/ndesktop-system")
    ([]{
        return crow::mustache::load_text("ndesktop-system.html");
    });

    CROW_ROUTE(app, "/novnc")
    ([]{
        return crow::mustache::load("novnc.html").render();
    });

    CROW_ROUTE(app, "/register")
    ([]{
        return crow::mustache::load("register.html").render();
    });

    CROW_ROUTE(app, "/scripteditor")
    ([]{
        return crow::mustache::load("scripteditor.html").render();
    });

    CROW_ROUTE(app, "/skymap")
    ([]{
        return crow::mustache::load("skymap.html").render();
    });

    CROW_ROUTE(app, "/system")
    ([]{
        return crow::mustache::load("system.html").render();
    });

    CROW_ROUTE(app, "/test")
    ([]{
        auto page = crow::mustache::load("test.html");
        return page.render();
    });

    CROW_ROUTE(app, "/webssh")
    ([]{
        return crow::mustache::load("webssh.html").render();
    });

    CROW_ROUTE(app, "/json/particles.json%22")
    ([](const crow::request&, crow::response& res) {
        //replace cat.jpg with your file path
        res.set_static_file_info("static/json/particles.json");
        res.end();
    });
}


#endif