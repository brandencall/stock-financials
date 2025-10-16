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
    std::string sourceTag;
};
