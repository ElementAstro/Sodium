#include "nlohmann/json.hpp"

using json = nlohmann::json;

// 定义 NormalStep 和 TargetStep 类
class NormalStep {
    // ...
};

class TargetStep {
    // ...
};

class PAAScript {
public:
    PAAScript(std::vector<json> paa_script_json) : script_json(paa_script_json) {
        // todo need to add a current step handler, and script re-saver.
        if (!__check_PAA_script()) {
            throw std::invalid_argument("PAA script check failed!");
        }
    }

    bool __check_PAA_script() {
        try {
            for (auto one_node : script_json) {
                if (one_node["device"] == "target") {
                    script_nodes.push_back(TargetStep(one_node));
                } else {
                    script_nodes.push_back(NormalStep(one_node));
                }
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    void PAA_exec_script() {
        for (auto one_node : script_nodes) {
            one_node.exec();
        }
    }

    std::vector<json> to_json() {
        std::vector<json> result;
        for (auto one_node : script_nodes) {
            result.push_back(one_node.to_json());
        }
        return result;
    }

    void update_script(std::vector<json> new_script) {
        // TODO
    }

    void save_script(std::string script_name) {
        // TODO
    }

    void compare_script(std::vector<json> new_script) {
        // TODO
    }

private:
    std::vector<json> script_json;
    std::vector<any> script_nodes;
};
