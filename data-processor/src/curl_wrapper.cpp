#include "curl_wrapper.h"
#include <chrono>
#include <curl/curl.h>
#include <stdexcept>
#include <string>
#include <thread>

CurlWrapper::CurlWrapper() {
    std::ifstream ifs("config.json");
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open config.json");
    }

    json data = json::parse(ifs);
    apiKey = data["key"];
}

size_t CurlWrapper::writeCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

size_t CurlWrapper::writeCallbackFile(void *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ofstream *file = static_cast<std::ofstream *>(userdata);
    file->write(static_cast<char *>(ptr), size * nmemb);
    return size * nmemb;
}

// TODO: break this method up
std::vector<db::model::StockPrice> CurlWrapper::getStockPriceData(const std::string &ticker) {
    const std::string url =
        "https://api.twelvedata.com/time_series?symbol=" + ticker + "&interval=1day&outputsize=5000&apikey=" + apiKey;
    int attempt = 0;
    int maxRetries = 3;
    std::vector<db::model::StockPrice> prices;
    std::string response;
    json j;

    while (attempt < maxRetries) {
        response = callAPI(url);
        j = json::parse(response);
        if (j.contains("code") && j["code"] == 404) {
            std::cout << "in the 404 check" << '\n';
            return prices;
        }

        std::string status = j["status"].get<std::string>();
        if (status == "error") {
            attempt++;
            if (attempt < maxRetries) {
                std::cout << "Rate limit hit for getting the stock Price data. Waiting 60 seconds for retry..." << '\n';
                j.clear();
                std::this_thread::sleep_for(std::chrono::seconds(60));
            } else {
                throw std::runtime_error("Exceeded retry limit. Could not process: " + ticker);
            }
        } else if (status != "ok") {
            throw std::runtime_error("The status isn't error or ok? status: " + status);
        } else {
            break;
        }
    }

    std::string currency = j["meta"]["currency"].get<std::string>();
    for (const auto &price : j["values"]) {
        db::model::StockPrice sp;
        sp.currency = currency;
        sp.date = price["datetime"].get<std::string>();
        sp.open = std::stod(price["open"].get<std::string>());
        sp.high = std::stod(price["high"].get<std::string>());
        sp.low = std::stod(price["low"].get<std::string>());
        sp.close = std::stod(price["close"].get<std::string>());
        sp.volume = std::stod(price["volume"].get<std::string>());
        prices.push_back(sp);
    }
    return prices;
}

std::vector<db::model::Company> CurlWrapper::getSecCompanyTickers() {
    const std::string url = "https://www.sec.gov/files/company_tickers.json";
    std::string response = callAPI(url);

    auto data = json::parse(response);
    std::vector<db::model::Company> companies;
    for (auto &[key, value] : data.items()) {
        companies.push_back(value.get<db::model::Company>());
    }
    return companies;
}

std::string CurlWrapper::callAPI(const std::string &url) {
    std::string response;

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "stock-data (brandencall@live.com)");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallbackJson);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        }
    }

    return response;
}

void CurlWrapper::getSecBulkData(std::filesystem::path path) {
    const char *url = "https://www.sec.gov/Archives/edgar/daily-index/xbrl/companyfacts.zip";
    const char *outputFile = path.c_str();

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize libcurl\n";
    }

    std::ofstream file(outputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open output file for writing\n";
        curl_easy_cleanup(curl);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallbackFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "stock-data (brandencall@live.com)");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    // 10 min timeout for the download
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    std::cout << "Downloading " << url << " ..." << std::endl;
    CURLcode res = curl_easy_perform(curl);

    file.close();
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
        throw "Error: Bulk Data download failed";
    }

    std::cout << "Download complete: " << outputFile << std::endl;
}
