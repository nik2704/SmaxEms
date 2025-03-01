#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

namespace smax_ns {

class Parser {
public:
    explicit Parser(const std::string& filename) : filename_(filename) {}

    json parseCSV(const std::string entity_type, const std::string action) {
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
                    properties[headers[i]] = "null";
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

        std::cout << result.dump(4) << std::endl;

        return result;
    }

private:
    std::string filename_;

    static std::vector<std::string> splitAndTrim(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(trim(token));
        }

        return tokens;
    }

    static std::string trim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
        return s;
    }
};

} // namespace smax_ns

