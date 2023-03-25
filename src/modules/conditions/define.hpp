#pragma once

#include <spdlog/spdlog.h>
#include "nlohmann/json.hpp"
#include <string>
#include <map>

using json = nlohmann::json;

class TargetStep {
    public:
    // define target_step here
};

class LoopCondition {
    public:
        std::string if_condition;
        std::map<std::string, int> params;
        TargetStep parent_target_step;

        LoopCondition(std::string if_cond, std::map<std::string, int> param, TargetStep target_step) :
            if_condition(if_cond),
            params(param),
            parent_target_step(target_step)
        {}

        bool check_value() {
            spdlog::info("!CONDITION! Default Condition checked as True");
            return true;
        }

        json to_json() {
            json j = {
                {"if", if_condition},
                {"params", params}
            };
            return j;
        }
};

class AfterLoopCondition {
    public:
        std::string if_condition;
        std::map<std::string, int> params;
        TargetStep parent_target_step;
        int count;

        AfterLoopCondition(std::string if_cond, std::map<std::string, int> param, TargetStep target_step) :
            if_condition(if_cond),
            params(param),
            parent_target_step(target_step),
            count(0)
        {}

        bool check_value_and_do(std::string loop_type="", void* loop_step_info=nullptr) {
            count++;
            spdlog::info("!AFTER LOOP! Default After Loop checked as True");
            spdlog::info("!AFTER LOOP! Default After Loop do EMPTY ACTION");
            return true;
        }

        json to_json() {
            json j = {
                {"if", if_condition},
                {"params", params}
            };
            return j;
        }
};