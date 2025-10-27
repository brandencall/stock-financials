#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace db::model {

struct StockPrice {
    int filingId;
    std::string date;
    std::string currency;
    double open;
    double high;
    double low;
    double close;
    double volume;
};

inline void to_json(json &j, const StockPrice &sp) {
    j = json{{"filingId", sp.filingId}, {"date", sp.date}, {"currency", sp.currency}, {"open", sp.open},
             {"high", sp.high},         {"low", sp.low},   {"close", sp.close},       {"volume", sp.volume}};
}

} // namespace db::model
