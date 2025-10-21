#pragma once

#include "controllers/company_controller.h"
#include "database.h"
#include "repositories/company_repository.h"
#include "services/company_service.h"
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
    service::CompanyService companyService;
};
