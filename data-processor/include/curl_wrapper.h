#pragma once
#include "models/company.h"
#include "models/stock_price.h"
#include <cstddef>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;


class CurlWrapper {

  public:
    explicit CurlWrapper();

    std::vector<db::model::StockPrice> getStockPriceData(const std::string &ticker);
    std::vector<db::model::Company> getSecCompanyTickers();
    void getSecBulkData(std::filesystem::path path);

  private:
    std::string apiKey;

    std::string callAPI(const std::string &url);
    static size_t writeCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output);
    static size_t writeCallbackFile(void *ptr, size_t size, size_t nmemb, void *userdata);
};
