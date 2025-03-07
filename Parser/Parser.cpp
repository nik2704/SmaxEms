#include "Parser.h"

using json = nlohmann::json;

namespace smax_ns {

Parser::Parser(const std::string& filename) : filename_(filename) {}

json Parser::parseCSV(const std::string entity_type, const std::string action) {
    std::lock_guard<std::mutex> lock(mtx_); // Ensure thread-safe access
    std::ifstream file(filename_);

    if (!file.is_open()) {
        throw std::runtime_error("ERROR opening file: " + filename_);
    }

    std::string line;
    std::vector<std::string> headers;
    json entities = json::array();

    // Read the first line as headers
    if (std::getline(file, line)) {
        headers = splitAndTrim(line, ',');
    }

    // Read the rest of the file line by line
    while (std::getline(file, line)) {
        std::vector<std::string> values = splitAndTrim(line, ',');
        json properties;

        // Map values to corresponding headers
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i < values.size() && !values[i].empty()) {
                properties[headers[i]] = values[i];
            } else {
                properties[headers[i]] = json(nullptr); // Assign null if no value exists
            }
        }

        // Construct entity JSON object
        entities.push_back({
            {"entity_type", entity_type},
            {"properties", properties}
        });
    }

    // Return the final JSON object
    return json{
        {"entities", entities},
        {"operation", action}
    };
}

std::vector<std::string> Parser::splitAndTrim(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    // Split string and trim each token
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

std::string Parser::trim(std::string s) {
    // Remove leading whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    // Remove trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

} // namespace smax_ns
