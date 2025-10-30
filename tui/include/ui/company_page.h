#pragma once

#include "../models.h"
#include "page.h"
#include <vector>

class Application;

class CompanyPage : public Page {
  public:
    explicit CompanyPage(Application &app, const Company &company);
    void render() override;
    void handleInput(int ch) override;
    bool needsRefresh() const override;

  private:
    Application &app;
    Company company;
    bool refreshNeeded;
    CompanyFinancials companyData;
};
