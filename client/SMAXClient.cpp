#include <sstream>
#include "SMAXClient.h"

namespace smax_ns {

std::unique_ptr<SMAXClient> SMAXClient::instance_ = nullptr;
std::once_flag SMAXClient::init_flag_;

SMAXClient& SMAXClient::getInstance(const ConnectionParameters& connection_props) {
    std::call_once(init_flag_, [&]() {
        instance_.reset(new SMAXClient(connection_props));
    });
    return *instance_;
}

SMAXClient::SMAXClient(const ConnectionParameters& connection_props) : connection_props_(connection_props) {}

std::string SMAXClient::getAuthorizationUrl() const {
    std::ostringstream url;
    url << connection_props_.getProtocol() << "://" << connection_props_.getHost();
    uint16_t port = connection_props_.getSecurePort();
    if (port != 80) {
        url << ":" << port;
    }
    url << "/auth/authentication-endpoint/authenticate/login?TENANTID=" << connection_props_.getTenant();
    return url.str();
}

std::string SMAXClient::getEmsUrl() const {
    std::ostringstream url;
    url << connection_props_.getProtocol() << "://" << connection_props_.getHost();
    uint16_t port = connection_props_.getPort();
    
    if (port != 80) {
        url << ":" << port;
    }
    
    url << "/rest/" << connection_props_.getTenant() << "/ems/" << connection_props_.getEntity() << "?layout=";
    url << connection_props_.getLayout();

    if (!connection_props_.getFilter().empty()) {
        url << "&filter=" << connection_props_.getFilter();
    }

    return url.str();
}

}
