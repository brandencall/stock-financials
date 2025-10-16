#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct FinancialFact {
    int filingId;
    std::string tag;
    std::string startDate;
    std::string endDate;
    double value;
    std::string unit;
    std::string source_tag;
};

inline void to_json(nlohmann::json &j, const FinancialFact &f) {}

inline void from_json(const nlohmann::json &j, FinancialFact &f) {}
