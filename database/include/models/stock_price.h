#pragma once

#include <string>

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

} // namespace db::model
