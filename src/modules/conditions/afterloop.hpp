#include "define.hpp"
#include <spdlog/spdlog.h>
#include <libnova/julian_day.h>
#include <libnova/solar.h>
#include <libnova/libnova.h>

class RecenterAfterLoop : public AfterLoopCondition {
public:
    RecenterAfterLoop(const std::string& if_condition, const std::map<std::string, double>& params, TargetStep* target_step)
        : AfterLoopCondition(if_condition, params, target_step) {
        this->action = new Goto_Center("telescope", "", {{"target_step", target_step}});
    }

    virtual async void check_value_and_do(const std::string& loop_type="", const void* loop_step_info=nullptr) override {
        this->count += 1;
        spdlog::info("!AFTER LOOP! Recenter after loop every check {}/{}", this->count, this->params["every"]);
        if (this->count % this->params["every"] == 0) {
            if (this->parent_target_step->newest_pic != nullptr) {
                // plate solve ra dec,
                double ra = 30;
                double dec = 30;

                // calculate difference
                ln_equ_posn this_center;
                this_center.ra = ra * 15.0; // convert hour angle to degrees
                this_center.dec = dec;
                ln_equ_posn target_center = {this->parent_target_step->target_coord.ra.to_value(), this->parent_target_step->target_coord.dec.to_value()};
                double distance = ln_get_angle_between_equ(&this_center, &target_center);

                if (distance * 3600 > this->params["threshold"]) {
                    spdlog::info("!AFTER LOOP! Recenter after loop check, center is out of threshold");

                    // do action chain
                    spdlog::info("!AFTER LOOP! Recenter after loop do recenter");
                    await this->action->do_action();
                } else {
                    spdlog::info("!AFTER LOOP! Recenter after loop checked False");
                }
            }
        }
    }

private:
    Goto_Center* action;
};

class AutoFocusAfterLoop : public AfterLoopCondition {
public:
    AutoFocusAfterLoop(const std::string& if_condition, const std::map<std::string, double>& params, TargetStep* target_step)
        : AfterLoopCondition(if_condition, params, target_step) {
    }

    virtual async void check_value_and_do(const std::string& loop_type="", const void* loop_step_info=nullptr) override {
        // Do nothing
    }
};

class MeridianFlipAfterLoop : public AfterLoopCondition {
public:
    MeridianFlipAfterLoop(const std::string& if_condition, const std::map<std::string, double>& params, TargetStep* target_step)
        : AfterLoopCondition(if_condition, params, target_step) {
    }

    virtual async void check_value_and_do(const std::string& loop_type="", const void* loop_step_info=nullptr) override {
        // Do nothing
    }
};
