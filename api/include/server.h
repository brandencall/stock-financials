#pragma once

#include "controllers/company_controller.h"
#include "database.h"
#include "repositories/company_repository.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "repositories/stock_price_repository.h"
#include "services/company_service.h"
#include "services/finacial_service.h"
#include <httplib.h>

class Server {
  public:
    Server(db::Database &db);
    void registerControllers();
    void run(int port);

  private:
    httplib::Server server;
    db::Database &db;
    db::repository::CompanyRepository companyRepo;
    db::repository::FilingRepository filingRepo;
    db::repository::FinancialFactRepository factRepo;
    db::repository::StockPriceRepository stockRepo;
    service::CompanyService companyService;
    controller::CompanyController companyController;
    service::FinancialService financialService;
};
