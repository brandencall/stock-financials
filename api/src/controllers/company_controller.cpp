#include "controllers/company_controller.h"

namespace controller {

CompanyController::CompanyController() {}

void CompanyController::registerRoutes(httplib::Server &server, service::CompanyService &companyService) {
    server.Get("/companies", [&](const httplib::Request &, httplib::Response &res) {
        auto companies = companyService.getAllCompanies();
        json j = json::array();
        for (auto &c : companies) {
            j.push_back({{"cik", c.cik}, {"ticker", c.ticker}, {"title", c.title}});
        }
        res.set_content(j.dump(), "application/json");
    });
}

} // namespace controller
