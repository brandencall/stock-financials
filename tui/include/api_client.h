#pragma once

#include "models.h"
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ApiClient {
  public:
    ApiClient() = default;

    std::vector<Company> getCompanies();

  private:
    static size_t writeCallbackJson(void *contents, size_t size, size_t nmemb, std::string *output);
    std::string callAPI(const std::string &url);
};
