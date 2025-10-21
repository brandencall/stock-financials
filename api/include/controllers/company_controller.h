#pragma once
#include "../services/company_service.h"
#include "models/company.h"
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace controller {

class CompanyController {

  public:
    explicit CompanyController();

    void registerRoutes(httplib::Server &server, service::CompanyService &service);
};
} // namespace controller
