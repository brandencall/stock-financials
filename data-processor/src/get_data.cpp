// #include "get_data.h"
//
//// TODO: Just make this a class. Wrapper around curl
//
// std::string getApiKey() {
//    std::ifstream ifs("config.json");
//    json data = json::parse(ifs);
//    return data["key"];
//}
//
// size_t WriteCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output) {
//    size_t totalSize = size * nmemb;
//    output->append((char *)contents, totalSize);
//    return totalSize;
//}
//
// size_t WriteCallbackFile(void *ptr, size_t size, size_t nmemb, void *userdata) {
//    std::ofstream *file = static_cast<std::ofstream *>(userdata);
//    file->write(static_cast<char *>(ptr), size * nmemb);
//    return size * nmemb;
//}
//
// std::string call_api(const std::string &url) {
//    std::string response;
//
//    CURL *curl = curl_easy_init();
//    if (curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//        curl_easy_setopt(curl, CURLOPT_USERAGENT, "stock-data (brandencall@live.com)");
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackJson);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//
//        CURLcode res = curl_easy_perform(curl);
//        curl_easy_cleanup(curl);
//
//        if (res != CURLE_OK) {
//            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
//        }
//    }
//
//    return response;
//}
//
// std::vector<db::model::StockPrice> get_stock_price_data(const std::string &ticker) {
//    std::string key = getApiKey();
//    const std::string url =
//        "https://api.twelvedata.com/time_series?symbol=" + ticker + "&interval=1day&outputsize=5000&apikey=" + key;
//    std::string response = call_api(url);
//    json j = json::parse(response);
//    std::string currency = j["meta"]["currency"].get<std::string>();
//    std::vector<db::model::StockPrice> prices;
//    for (const auto &price : j["values"]) {
//        db::model::StockPrice sp;
//        sp.currency = currency;
//        sp.date = price["datetime"].get<std::string>();
//        sp.open = std::stod(price["open"].get<std::string>());
//        sp.high = std::stod(price["high"].get<std::string>());
//        sp.low = std::stod(price["low"].get<std::string>());
//        sp.close = std::stod(price["close"].get<std::string>());
//        sp.volume = std::stod(price["volume"].get<std::string>());
//        prices.push_back(sp);
//    }
//    return prices;
//}
//
// std::vector<db::model::Company> get_sec_company_tickers() {
//    const std::string url = "https://www.sec.gov/files/company_tickers.json";
//    std::string response = call_api(url);
//
//    auto data = json::parse(response);
//    std::vector<db::model::Company> companies;
//    for (auto &[key, value] : data.items()) {
//        companies.push_back(value.get<db::model::Company>());
//    }
//    return companies;
//}
//
// void get_sec_bulk_data(std::filesystem::path path) {
//    const char *url = "https://www.sec.gov/Archives/edgar/daily-index/xbrl/companyfacts.zip";
//    const char *outputFile = path.c_str();
//
//    CURL *curl = curl_easy_init();
//    if (!curl) {
//        std::cerr << "Failed to initialize libcurl\n";
//    }
//
//    std::ofstream file(outputFile, std::ios::binary);
//    if (!file.is_open()) {
//        std::cerr << "Failed to open output file for writing\n";
//        curl_easy_cleanup(curl);
//    }
//
//    curl_easy_setopt(curl, CURLOPT_URL, url);
//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackFile);
//    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
//    curl_easy_setopt(curl, CURLOPT_USERAGENT, "stock-data (brandencall@live.com)");
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
//    // 10 min timeout for the download
//    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
//    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
//
//    std::cout << "Downloading " << url << " ..." << std::endl;
//    CURLcode res = curl_easy_perform(curl);
//
//    file.close();
//    curl_easy_cleanup(curl);
//
//    if (res != CURLE_OK) {
//        std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
//        throw "Error: Bulk Data download failed";
//    }
//
//    std::cout << "Download complete: " << outputFile << std::endl;
//}
