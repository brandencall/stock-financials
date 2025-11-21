#pragma once
#include "api_client.h"
#include "models.h"
#include "ui/page.h"
#include <memory>
#include <ncurses.h>
#include <vector>

class SearchPage;
class CompanyPage;

class Application {
  public:
    explicit Application(ApiClient &apiClient);

    void initUi();
    void run();
    void shutdownUi();

    ApiClient &getApiClient() { return apiClient; }
    const std::vector<Company> &getCompanies();
    void switchToCompanyPage(const Company &company);
    void switchPage(std::unique_ptr<Page> page);

  private:
    ApiClient &apiClient;
    // May want to move caching into a seperate dedicated class
    std::vector<Company> cachedCompanies;
    bool companiesLoaded = false;

    std::unique_ptr<Page> currentPage;

    void switchToSearchPage();
};
