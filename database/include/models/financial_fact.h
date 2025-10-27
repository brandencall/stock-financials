#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace db::model {

struct FinancialFact {
    int filingId;
    std::string tag;
    std::string startDate;
    std::string endDate;
    double value;
    std::string unit;
    std::string sourceTag;
};

inline void to_json(json &j, const FinancialFact &f) {
    j = json{
        {"filingId", f.filingId}, {"tag", f.tag},   {"startDate", f.startDate}, {"endDate", f.endDate},
        {"value", f.value},       {"unit", f.unit}, {"sourceTag", f.sourceTag},
    };
}
} // namespace db::model
