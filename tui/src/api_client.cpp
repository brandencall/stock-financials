#include "api_client.h"
#include "models.h"
#include <string>
#include <vector>

size_t ApiClient::writeCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

std::vector<Company> ApiClient::getCompanies() {
    std::string companiesJson = callAPI("http://10.0.0.210:8080/companies");
    json j = json::parse(companiesJson);
    return j.get<std::vector<Company>>();
}

CompanyFinancials ApiClient::getCompaniesAnnualFinancials(const std::string &cik, int limit) {
    std::string limitStr = std::to_string(limit);
    std::string url = "http://10.0.0.210:8080/companies/" + cik + "/financials?period=annual&limit=" + limitStr;
    json j = json::parse(callAPI(url));
    return j.get<CompanyFinancials>();
}

std::string ApiClient::callAPI(const std::string &url) {
    std::string response;

    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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
