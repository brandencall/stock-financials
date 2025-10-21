#include "controllers/company_controller.h"

namespace controller {

CompanyController::CompanyController(service::CompanyService &companyService) : companyService(companyService) {}

void CompanyController::registerRoutes(httplib::Server &server) {
    server.Get("/companies",
               [this](const httplib::Request &req, httplib::Response &res) { this->getCompanies(req, res); });

    server.Get(R"(/company)",
               [this](const httplib::Request &req, httplib::Response &res) { this->getCompany(req, res); });
}

void CompanyController::getCompanies(const httplib::Request &, httplib::Response &res) {
    auto companies = companyService.getAllCompanies();
    json j = json::array();
    for (auto &c : companies) {
        j.push_back({{"cik", c.cik}, {"ticker", c.ticker}, {"title", c.title}});
    }
    res.set_content(j.dump(), "application/json");
}

void CompanyController::getCompany(const httplib::Request &req, httplib::Response &res) {
    if (!req.has_param("cik")) {
        res.status = 404;
        res.set_content(R"({"error": "Missing cik paramter"})", "application/json");
    }
    std::string cik = req.get_param_value("cik");
    std::optional<db::model::Company> company = companyService.getCompanyByCIK(cik);
    if (company != std::nullopt) {
        json j = {{"cik", company.value().cik}, {"ticker", company.value().ticker}, {"title", company.value().title}};
        res.set_content(j.dump(), "application/json");
    } else {
        res.status = 404;
        res.set_content(R"({"error": "Company does not exist"})", "application/json");
    }
}

} // namespace controller
