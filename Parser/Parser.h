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

/**
 * @class Parser
 * @brief Parses CSV files and converts data into JSON format.
 */
class Parser {
public:
    /**
     * @brief Constructor to initialize the parser with a CSV file.
     * @param filename Name of the CSV file to parse.
     */
    explicit Parser(const std::string& filename);

    /**
     * @brief Parses the CSV file and converts it into JSON format.
     * @param entity_type Type of the entity to be included in JSON output.
     * @param action Specifies the operation associated with the parsed data.
     * @return JSON object containing parsed entities and operation information.
     * @throws std::runtime_error If the file cannot be opened.
     */
    json parseCSV(const std::string entity_type, const std::string action);

private:
    std::string filename_;  ///< Name of the CSV file to be parsed.
    mutable std::mutex mtx_; ///< Mutex for thread-safe file access.

    /**
     * @brief Splits a string by a given delimiter and trims whitespace from each token.
     * @param str Input string to split.
     * @param delimiter Character used as a separator.
     * @return A vector of trimmed substrings.
     */
    static std::vector<std::string> splitAndTrim(const std::string& str, char delimiter);

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param s Input string to trim.
     * @return Trimmed string.
     */
    static std::string trim(std::string s);
};

} // namespace smax_ns

