#pragma once
#include "../services/company_service.h"
#include "models/company.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <optional>

using json = nlohmann::json;

namespace controller {

class CompanyController {

  public:
    explicit CompanyController(service::CompanyService &companyService);

    void registerRoutes(httplib::Server &server);

  private:
    service::CompanyService &companyService;

    void getCompanies(const httplib::Request &, httplib::Response &res);
    void getCompany(const httplib::Request &req, httplib::Response &res);
};
} // namespace controller
