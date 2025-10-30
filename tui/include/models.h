#pragma once

#include "nlohmann/json.hpp"
#include <algorithm>
#include <cctype>
#include <string>

using json = nlohmann::json;

struct Company {
    std::string cik;
    std::string ticker;
    std::string title;
    std::string queryString;
};

inline void from_json(const nlohmann::json &j, Company &c) {
    j.at("cik").get_to(c.cik);
    j.at("ticker").get_to(c.ticker);
    j.at("title").get_to(c.title);
    std::string combined = c.ticker + " " + c.title;
    std::transform(combined.begin(), combined.end(), combined.begin(), ::tolower);
    c.queryString = combined;
}
