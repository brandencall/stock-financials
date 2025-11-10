#pragma once
#include "filing.h"
#include "financial_fact.h"
#include "stock_price.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace db::model {

struct FinancialReport {
    Filing filing;
    std::vector<FinancialFact> facts;
    std::optional<StockPrice> stockPrice;
};

inline void to_json(json &j, const FinancialReport &fr) {
    j = json{
        {"filing", fr.filing},
        {"facts", fr.facts},
        {"stockPrice", fr.stockPrice},
    };
}

} // namespace db::model
