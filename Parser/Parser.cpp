#include "Parser.h"


using json = nlohmann::json;

namespace smax_ns {

Parser::Parser(const std::string& filename) : filename_(filename) {}

json Parser::parseCSV(const std::string entity_type, const std::string action) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::ifstream file(filename_);

    if (!file.is_open()) {
        throw std::runtime_error("ERROR open file: " + filename_);
    }

    std::string line;
    std::vector<std::string> headers;
    json entities = json::array();

    if (std::getline(file, line)) {
        headers = splitAndTrim(line, ',');
    }

    while (std::getline(file, line)) {
        std::vector<std::string> values = splitAndTrim(line, ',');
        json properties;

        for (size_t i = 0; i < headers.size(); ++i) {
            if (i < values.size() && !values[i].empty()) {
                properties[headers[i]] = values[i];
            } else {
                properties[headers[i]] = json(nullptr);
            }
        }

        entities.push_back({
            {"entity_type", entity_type},
            {"properties", properties}
        });
    }

    json result = json{
        {"entities", entities},
        {"operation", action}
    };

    return result;
}

std::vector<std::string> Parser::splitAndTrim(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

std::string Parser::trim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

} // namespace smax_ns

