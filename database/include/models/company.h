#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace db::model {

struct Company {
    std::string cik;
    std::string ticker;
    std::string title;
};

inline void to_json(json &j, const Company &c) { j = json{{"cik", c.cik}, {"ticker", c.ticker}, {"title", c.title}}; }

inline void from_json(const nlohmann::json &j, Company &c) {
    c.cik = std::to_string(j.at("cik_str").get<long long>());
    j.at("ticker").get_to(c.ticker);
    j.at("title").get_to(c.title);
}

} // namespace db::model
