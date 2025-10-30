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
};

inline void from_json(const nlohmann::json &j, Company &c) {
    j.at("cik").get_to(c.cik);
    j.at("ticker").get_to(c.ticker);
    j.at("title").get_to(c.title);
}
