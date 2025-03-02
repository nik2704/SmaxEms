#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <mutex>

using json = nlohmann::json;

namespace smax_ns {

class Parser {
public:
    explicit Parser(const std::string& filename);

    json parseCSV(const std::string entity_type, const std::string action);

private:
    std::string filename_;
    mutable std::mutex mtx_;

    static std::vector<std::string> splitAndTrim(const std::string& str, char delimiter);

    static std::string trim(std::string s);
};

} // namespace smax_ns

