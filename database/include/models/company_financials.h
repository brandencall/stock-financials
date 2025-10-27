#pragma once
#include "company.h"
#include "financial_report.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace db::model {

struct CompanyFinancials {
    Company company;
    std::vector<FinancialReport> reports;
};

inline void to_json(json &j, const CompanyFinancials &cf) {
    j = json{
        {"company", cf.company},
        {"reports", cf.reports},
    };
}

} // namespace db::model
