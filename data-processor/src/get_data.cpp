#include "get_data.h"
#include "models/company.h"
#include <vector>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

std::vector<Company> get_sec_company_tickers() {
    const std::string url = "https://www.sec.gov/files/company_tickers.json";
    std::string response;

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "stock-data (brandencall@live.com)");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        }
    }

    // Parse JSON
    auto data = json::parse(response);
    std::vector<Company> companies;
    for (auto &[key, value] : data.items()) {
        companies.push_back(value.get<Company>());
    }
    return companies;
}
