#include "controllers/company_controller.h"
#include "models/company_financials.h"
#include <stdexcept>

namespace controller {

CompanyController::CompanyController(service::CompanyService &companyService,
                                     service::FinancialService &financialService)
    : companyService(companyService), financialService(financialService) {}

void CompanyController::registerRoutes(httplib::Server &server) {
    server.Get("/companies",
               [this](const httplib::Request &req, httplib::Response &res) { this->getCompanies(req, res); });

    server.Get(R"(/companies/(\d+))",
               [this](const httplib::Request &req, httplib::Response &res) { this->getCompany(req, res); });

    server.Get(R"(/companies/(\d+)/financials)",
               [this](const httplib::Request &req, httplib::Response &res) { this->getFinancials(req, res); });
}

void CompanyController::getCompanies(const httplib::Request &, httplib::Response &res) {
    auto companies = companyService.getAllCompanies();
    json j = json::array();
    for (auto &c : companies) {
        json jsonCompany = c;
        j.push_back(jsonCompany);
    }
    res.set_content(j.dump(), "application/json");
}

void CompanyController::getCompany(const httplib::Request &req, httplib::Response &res) {
    if (req.matches.size() > 1) {
        std::string cik = req.matches[1];
        std::optional<db::model::Company> company = companyService.getCompanyByCIK(cik);
        if (company != std::nullopt) {
            json j = company;
            res.set_content(j.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content(R"({"error": "Company does not exist"})", "application/json");
        }
    } else {
        res.status = 404;
        res.set_content(R"({"error": "Missing cik paramter"})", "application/json");
    }
}

void CompanyController::getFinancials(const httplib::Request &req, httplib::Response &res) {

    if (req.matches.size() > 1) {
        std::string cik = req.matches[1];
        std::string period = "annual";
        int limit = 10;
        if (req.has_param("period")) {
            period = req.get_param_value("period");
        }
        if (period != "annual" && period != "quarterly") {
            res.status = 404;
            res.set_content(R"({"error": "inproper period paramter"})", "application/json");
            return;
        }
        if (req.has_param("limit")) {
            try {
                limit = std::stoi(req.get_param_value("limit"));
            } catch (const std::invalid_argument &e) {
                res.status = 404;
                res.set_content(R"({"error": "Invalid limit paramter. Paramter must be an int"})", "application/json");
                return;
            } catch (const std::out_of_range &e) {
                res.status = 404;
                res.set_content(R"({"error": "Invalid limit paramter. Paramter must be an int"})", "application/json");
                return;
            }
        }
        std::optional<db::model::CompanyFinancials> companyFinancials =
            financialService.getByCikAndPeriod(cik, period, limit);
        if (companyFinancials != std::nullopt) {
            json j = companyFinancials.value();
            res.set_content(j.dump(), "application/json");
            return;
        } else {
            res.status = 404;
            res.set_content(R"({"error": "Company Financials doesn't exist..."})", "application/json");
            return;
        }
    } else {
        res.status = 404;
        res.set_content(R"({"error": "Missing cik paramter"})", "application/json");
        return;
    }
}

} // namespace controller
