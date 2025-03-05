#include "ConnectionProperties.h"
#include <sstream>
#include <mutex>
#include <unordered_map>

namespace smax_ns {

std::unique_ptr<ConnectionParameters> ConnectionParameters::instance_ = nullptr;
std::once_flag ConnectionParameters::init_flag_;

Action parseAction(const std::string& action_str) {
    static const std::unordered_map<std::string, Action> action_map = {
        {"GET", Action::GET},
        {"CREATE", Action::CREATE},
        {"JSON", Action::JSON},
        {"UPDATE", Action::UPDATE}
    };

    auto it = action_map.find(action_str);
    return (it != action_map.end()) ? it->second : Action::UNKNOW;
}

std::shared_ptr<std::vector<std::string>> splitString(const std::string& input, char delimiter) {
    auto result = std::make_shared<std::vector<std::string>>();
    std::stringstream ss(input);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result->push_back(token);
    }
    
    return result;
}

std::string ConnectionParameters::actionToString(Action action) {
    switch (action) {
        case Action::GET: return "GET";
        case Action::CREATE: return "CREATE";
        case Action::JSON: return "JSON";
        case Action::UPDATE: return "UPDATE";
        default: return "UNKNOWN";
    }
}

ConnectionParameters& ConnectionParameters::getInstance(const InputValues& input_values) {
    std::call_once(init_flag_, [&]() {
        instance_.reset(new ConnectionParameters(input_values));
    });
    return *instance_;
}

ConnectionParameters& ConnectionParameters::getInstance() {
    if (!instance_) {
        throw std::runtime_error("ConnectionParameters instance not initialized with InputValues.");
    }
    return *instance_;
}

ConnectionParameters::ConnectionParameters(const InputValues& input_values)
    : protocol_(input_values.protocol),
      host_(input_values.host),
      port_(input_values.port),
      secure_port_(input_values.secure_port),
      tenant_(input_values.tenant),
      entity_(input_values.entity),
      layout_(input_values.layout),
      username_(input_values.username),
      password_(input_values.password),
      filter_(input_values.filter),
      action_(parseAction(input_values.action)),
      csv_(input_values.csv),
      verbose_(input_values.verbose),
      json_action_field_(input_values.json_action_field),
      json_action_fields_list_(splitString(input_values.json_action_field)),
      json_action_output_(input_values.json_action_output),
      json_action_output_folder_(input_values.json_action_output_folder) {}

const std::string& ConnectionParameters::getProtocol() const { return protocol_; }
const std::string& ConnectionParameters::getHost() const { return host_; }
uint16_t ConnectionParameters::getPort() const { return port_; }
uint16_t ConnectionParameters::getSecurePort() const { return secure_port_; }
std::size_t ConnectionParameters::getTenant() const { return tenant_; }
const std::string& ConnectionParameters::getEntity() const { return entity_; }
const std::string& ConnectionParameters::getLayout() const { return layout_; }
const std::string& ConnectionParameters::getFilter() const { return filter_; }
const std::string& ConnectionParameters::getUserName() const { return username_; }
const std::string& ConnectionParameters::getPassword() const { return password_; }
const Action& ConnectionParameters::getAction() const { return action_; }
std::string ConnectionParameters::getActionAsString() const { return actionToString(action_); }
const std::string& ConnectionParameters::getCSVfilename() const { return csv_; }
bool ConnectionParameters::isVerbose() const { return verbose_; }
const std::string& ConnectionParameters::getJsonActionField() const { return json_action_field_; }
const std::string& ConnectionParameters::getJsonActionOutput() const { return json_action_output_; }
const std::string& ConnectionParameters::getJsonActionOutputFolder() const { return json_action_output_folder_; }
const std::shared_ptr<std::vector<std::string>>& ConnectionParameters::getJsonActionFieldsList() const {
    return json_action_fields_list_;
}



} // namespace smax_ns
