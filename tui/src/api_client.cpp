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

CompanyFinancials ApiClient::getCompaniesFinancials(const std::string &cik, const std::string &period, int limit) {
    return getCompaniesFinancials(cik, period, {}, limit);
}

CompanyFinancials ApiClient::getCompaniesFinancials(const std::string &cik, const std::string &period,
                                                    const std::vector<std::string> &facts, int limit) {
    std::string url = "http://10.0.0.210:8080/companies/" + cik + "/financials?period=" + period +
                      "&limit=" + std::to_string(limit) + "&facts=";
    for (const auto &fact : facts) {
        url += formatSpacesForUrl(fact) + ",";
    }
    json j = json::parse(callAPI(url));
    return j.get<CompanyFinancials>();
}

std::string ApiClient::formatSpacesForUrl(const std::string &str) {
    std::string result;
    for (const auto &c : str) {
        if (c != ' ') {
            result += c;
        } else {
            result += "%20";
        }
    }
    return result;
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
