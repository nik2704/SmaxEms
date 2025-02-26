#include "ConnectionProperties.h"
#include <mutex>

namespace smax_ns {

std::unique_ptr<ConnectionParameters> ConnectionParameters::instance_ = nullptr;
std::once_flag ConnectionParameters::init_flag_;

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
      filter_(input_values.filter) {}

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

} // namespace smax_ns
