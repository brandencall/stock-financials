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

struct StockPrice {
    std::string date;
    std::string currency;
    double open;
    double high;
    double low;
    double close;
    double volume;
};

inline void from_json(const nlohmann::json &j, StockPrice &sp) {
    j.at("date").get_to(sp.date);
    j.at("currency").get_to(sp.currency);
    j.at("open").get_to(sp.open);
    j.at("high").get_to(sp.high);
    j.at("low").get_to(sp.low);
    j.at("close").get_to(sp.close);
    j.at("volume").get_to(sp.volume);
}

struct Filing {
    std::string form;
    int fy;
    std::string fp;
    std::string filedDate;
};

inline void from_json(const nlohmann::json &j, Filing &f) {
    j.at("form").get_to(f.form);
    j.at("fy").get_to(f.fy);
    j.at("fp").get_to(f.fp);
    j.at("filedDate").get_to(f.filedDate);
}

struct FinancialFact {
    std::string tag;
    std::string startDate;
    std::string endDate;
    double value;
    std::string unit;
    std::string sourceTag;
};

inline void from_json(const nlohmann::json &j, FinancialFact &ff) {
    j.at("tag").get_to(ff.tag);
    j.at("startDate").get_to(ff.startDate);
    j.at("endDate").get_to(ff.endDate);
    j.at("value").get_to(ff.value);
    j.at("unit").get_to(ff.unit);
    j.at("sourceTag").get_to(ff.sourceTag);
}

struct FinancialReport {
    Filing filing;
    std::vector<FinancialFact> facts;
    StockPrice stockPrice;
};

inline void from_json(const nlohmann::json &j, FinancialReport &fr) {
    j.at("filing").get_to(fr.filing);
    j.at("facts").get_to(fr.facts);
    j.at("stockPrice").get_to(fr.stockPrice);
}

struct CompanyFinancials {
    Company company;
    std::vector<FinancialReport> reports;
};

inline void from_json(const nlohmann::json &j, CompanyFinancials &cf) {
    j.at("company").get_to(cf.company);
    j.at("reports").get_to(cf.reports);
}
