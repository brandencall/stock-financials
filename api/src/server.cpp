#include "server.h"
#include "services/filing_service.h"

Server::Server(db::Database &db)
    : db(db), companyRepo(db), filingRepo(db), factRepo(db), stockRepo(db), companyService(companyRepo),
      companyController(companyService, financialService, filingService),
      financialService(companyRepo, filingRepo, factRepo, stockRepo), filingService(filingRepo) {}

void Server::registerControllers() { companyController.registerRoutes(server); }

void Server::run(int port) { server.listen("0.0.0.0", port); }
