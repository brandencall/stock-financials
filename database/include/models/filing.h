#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct Filing {
    int filingId;
    std::string accession;
    std::string cik;
    std::string form;
    int fy;
    std::string fp;
    std::string filed_date;
};

inline void to_json(nlohmann::json &j, const Filing &f) {}

inline void from_json(const nlohmann::json &j, Filing &f) {}
