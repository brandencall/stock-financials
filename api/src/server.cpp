#include "server.h"
#include "repositories/filing_repository.h"
#include "repositories/financial_fact_repository.h"
#include "repositories/stock_price_repository.h"
#include "services/finacial_service.h"

Server::Server(db::Database &db)
    : db(db), companyRepo(db), filingRepo(db), factRepo(db), stockRepo(db), companyService(companyRepo),
      companyController(companyService, financialService),
      financialService(companyRepo, filingRepo, factRepo, stockRepo) {}

void Server::registerControllers() { companyController.registerRoutes(server); }

void Server::run(int port) { server.listen("0.0.0.0", port); }
