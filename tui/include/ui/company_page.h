#pragma once

#include "../models.h"
#include "page.h"
#include "ui_utils.h"
#include <algorithm>
#include <format>
#include <iomanip>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <vector>

class Application;

struct CompanyHeaderData {
    std::string fiscalYear;
    double price;
    double peRatio;
    double dividend;
    double marketCap;
};

class CompanyPage : public Page {
  public:
    explicit CompanyPage(Application &app, const Company &company);
    ~CompanyPage();
    void render() override;
    void handleInput(int ch) override;
    bool needsRefresh() const override;

  private:
    Application &app;
    Company company;
    bool refreshNeeded;
    CompanyFinancials companyData;

    int headerHeight = 3;
    int headerWidth = 0;
    int headerY = 0;
    int headerX = 0;

    int revenueBarHeight = 7;
    int revenueBarWidth = (COLS / 2) - 1;
    int revenueBarY = 4;
    int revenueBarX = 1;

    int operatingIncomeHeight = 7;
    int operatingIncomeWidth = (COLS / 2) - 1;
    int operatingIncomeY = 4;
    int operatingIncomeX = (COLS / 2) + 1;

    WINDOW *mainWin;
    WINDOW *headerWin;
    WINDOW *revenueBarWin;
    WINDOW *operatingIncomeWin;

    void renderTitle();
    void renderNoDataError();
    void renderHeader();
    void renderRevenueBar();
    void renderOperatingIncomeBar();
    CompanyHeaderData getHeaderData();
};
