/*
 * papsdefine.hpp
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
 
Description: Programming Astrophotography Perfect Script Definitions
 
**************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

using namespace boost::asio;
using namespace boost::posix_time;

class Stepper {
public:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
    std::vector<Condition> conditions;

    Stepper(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms, const std::vector<Condition>& cnds) : 
        device(dev), instruction(inst), params(pms), conditions(cnds) {}

    virtual ~Stepper() {}

    virtual void to_json() = 0;

    bool check_condition() {
        for (auto condition : conditions) {
            if (!condition.check()) {
                return false;
            }
        }
        return true;
    }

    virtual bool check_loop_condition() {
        return true;
    }
};

class Action {
public:
    virtual void do_action() = 0;

    virtual ~Action() {}
};

class Telescope_Action : public Action {
public:
    Telescope_Action(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms) :
        device(dev), instruction(inst), params(pms) {}

    virtual ~Telescope_Action() {}

    void do_action() {
        // TODO: implement telescope action
    }

private:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
};

class Camera_Action : public Action {
public:
    Camera_Action(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms) :
        device(dev), instruction(inst), params(pms) {}

    virtual ~Camera_Action() {}

    void do_action() {
        // TODO: implement camera action
    }

private:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
};

class Focus_Action : public Action {
public:
    Focus_Action(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms) :
        device(dev), instruction(inst), params(pms) {}

    virtual ~Focus_Action() {}

    void do_action() {
        // TODO: implement focus action
    }

private:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
};

class Filter_Action : public Action {
public:
    Filter_Action(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms) :
        device(dev), instruction(inst), params(pms) {}

    virtual ~Filter_Action() {}

    void do_action() {
        // TODO: implement filter action
    }

private:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
};

class Guider_Action : public Action {
public:
    Guider_Action(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms) :
        device(dev), instruction(inst), params(pms) {}

    virtual ~Guider_Action() {}

    void do_action() {
        // TODO: implement guider action
    }

private:
    std::string device;
    std::string instruction;
    std::map<std::string, std::string> params;
};

class TargetStep : public Stepper {
public:
    std::string target_name;
    double ra;
    double dec;
    SkyCoord target_coord;
    std::shared_ptr<newest_pic> newest_pic;
    std::vector<std::shared_ptr<Stepper>> children;
    std::vector<std::shared_ptr<AfterLoop>> after_loop;

    TargetStep(const std::string& tar_name, const double& r, const double& d, const std::map<std::string, std::string>& pms, 
        std::shared_ptr<newest_pic> npc, const std::vector<std::shared_ptr<Stepper>>& chds, const std::vector<std::shared_ptr<AfterLoop>>& af_lp) :
        target_name(tar_name), ra(r), dec(d), target_coord(SkyCoord(ra * u.degree, dec * u.degree)),
        newest_pic(npc), children(chds), after_loop(af_lp), Stepper("target", "", pms, std::vector<Condition>()) {}

    ~TargetStep() {}

    bool check_loop_condition() {
        // TODO: implement loop condition check for target step
        return true;
    }

    void to_json() {
        // TODO: implement to_json function for target step
    }

private:
    std::shared_ptr<AfterLoop> initial_after_loop(const std::string& if_con, const std::map<std::string, std::string>& pms, TargetStep* tar_step) {
        // TODO: implement initial_after_loop function for target step
    }
};

class NormalStep : public Stepper {
public:
    std::string type;
    int count;
    std::unique_ptr<Action> action;

    NormalStep(const std::string& dev, const std::string& inst, const std::map<std::string, std::string>& pms, 
        const std::vector<Condition>& cnds, const std::string& ty, const int& cnt, std::unique_ptr<Action> act) :
        type(ty), count(cnt), action(std::move(act)), Stepper(dev, inst, pms, cnds) {}

    ~NormalStep() {}

    bool check_loop_condition() {
        // TODO: implement loop condition check for normal step
        return true;
    }

    void to_json() {
        // TODO: implement to_json function for normal step
    }
};
