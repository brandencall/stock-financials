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
    bool needsRefresh() const override;

  private:
    Application &app;
    Company company;
    bool refreshNeeded;
    std::vector<std::string> financialFactList;
    CompanyFinancials companyData;
    CompanyFinancials latestCompanyData;

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

    int epsHeight = 7;
    int epsWidth = (COLS / 4) - 1;
    int epsY = 12;
    int epsX = 1;

    int peHeight = 7;
    int peWidth = (COLS / 4) - 1;
    int peY = 12;
    int peX = (COLS / 4) + 1;

    int cashToDebtHeight = 7;
    int cashToDebtWidth = (COLS / 2) - 1;
    int cashToDebtY = 12;
    int cashToDebtX = (COLS / 2) + 1;

    int latestFinancialsHeight = 10;
    int latestFinancialsWidth = COLS - 1;
    int latestFinancialsY = 19;
    int latestFinancialsX = 1;

    WINDOW *mainWin;
    WINDOW *headerWin;
    WINDOW *revenueBarWin;
    WINDOW *operatingIncomeWin;
    WINDOW *epsWin;
    WINDOW *peWin;
    WINDOW *cashToDebtWin;
    WINDOW *latestFinancialsWin;

    void renderBarGraphs();
    void renderMainWindowTitle();
    double calculateGrowthRate(double currentValue, double previousValue);
    void renderRevenue(const std::vector<DataPoint> &revenuePoints, const std::string &currency);
    void renderOperatingIncome(const std::vector<DataPoint> &operatingIncomePoints, const std::string &currency);
    void renderEPS(const std::vector<DataPoint> &epsPoints, const std::string &unit);
    void renderPe(const std::vector<DataPoint> &pePoints);
    void renderNoDataError();
    void renderHeader();
    void renderCashToDebtBar();
    void renderFinancialsWin();
    void renderFinancialFact(const FinancialFact &fact, int &rowCount, int &colCount);
    CompanyHeaderData getHeaderData();
};
