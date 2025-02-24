#include <string>
#include <mutex>
#include "ConnectionProperties.h"

namespace smax_ns {

ConnectionParameters* ConnectionParameters::instance_ = nullptr;

inline std::string ToLower(const std::string& input) {
    std::string result = input;

    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    }

    return result;
}

ConnectionParameters& ConnectionParameters::GetInstance(const InputValues& input_values) {
    static std::once_flag init_flag;
    std::call_once(init_flag, [&]() {
        instance_ = new ConnectionParameters(input_values);
    });

    return *instance_;
}

ConnectionParameters& ConnectionParameters::GetInstance() {
    if (!instance_) {
        throw std::runtime_error("ConnectionParameters is not initialized.");
    }

    return *instance_;
}

ConnectionParameters::ConnectionParameters(const InputValues& input_values) :
    protocol_(ToLower(input_values.protocol)),
    host_(ToLower(input_values.host)),
    port_(input_values.port),
    tenant_(input_values.tenant),
    entity_(input_values.entity),
    layout_(input_values.layout)
{}

const std::string& ConnectionParameters::GetProtocol() const { return protocol_; }
const std::string& ConnectionParameters::GetHost() const { return host_; }
uint16_t ConnectionParameters::GetPort() const { return port_; }
std::size_t ConnectionParameters::GetTenant() const { return tenant_; }
const std::string& ConnectionParameters::GetEntity() const { return entity_; }
const std::string& ConnectionParameters::GetLayout() const { return layout_; }

} // namespace smax_ns
