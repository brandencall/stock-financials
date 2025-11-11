#pragma once

#include "models.h"
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class ApiClient {
  public:
    ApiClient() = default;

    std::vector<Company> getCompanies();
    CompanyFinancials getCompaniesAnnualFinancials(const std::string &cik, int limit);
    CompanyFinancials getCompaniesFinancials(const std::string &cik, const std::string &period, int limit);
    CompanyFinancials getCompaniesFinancials(const std::string &cik, const std::string &period,
                                             const std::vector<std::string> &facts, int limit);

  private:
    static size_t writeCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output);
    std::string formatSpacesForUrl(const std::string &str);
    std::string callAPI(const std::string &url);
};
