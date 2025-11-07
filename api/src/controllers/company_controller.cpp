#include "controllers/company_controller.h"
#include "models/company_financials.h"
#include <sstream>
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
    if (req.matches.size() <= 1) {
        return sendError(res, 404, "Missing cik parameter");
    }
    std::string cik = req.matches[1];
    std::optional<db::model::Company> company = companyService.getCompanyByCIK(cik);
    if (company != std::nullopt) {
        json j = company;
        res.set_content(j.dump(), "application/json");
    } else {
        res.status = 404;
        res.set_content(R"({"error": "Company does not exist"})", "application/json");
    }
}

void CompanyController::getFinancials(const httplib::Request &req, httplib::Response &res) {
    if (req.matches.size() <= 1) {
        return sendError(res, 404, "Missing cik paramter");
    }

    std::string cik = req.matches[1];
    std::string period = req.has_param("period") ? req.get_param_value("period") : "annual";
    std::optional<int> limit = parseLimit(req);
    std::vector<std::string> facts = parseFacts(req);

    if (period != "annual" && period != "quarterly") {
        return sendError(res, 404, "Inproper period parameter");
    }

    if (!limit.has_value() && req.has_param("limit")) {
        return sendError(res, 404, "Invalid limit parameter. Must be an integer");
    }

    std::optional<db::model::CompanyFinancials> companyFinancials =
        limit.has_value() ? financialService.getByCikAndPeriod(cik, period, limit.value(), facts)
                          : financialService.getAllByCikAndPeriod(cik, period, facts);

    if (!companyFinancials) {
        return sendError(res, 404, "Company Financials doesn't exist...");
    }

    json j = *companyFinancials;
    res.status = 200;
    res.set_content(j.dump(), "application/json");
}

void CompanyController::sendError(httplib::Response &res, int status, const std::string &message) {
    res.status = status;
    json j = {{"error", message}};
    res.set_content(j.dump(), "application/json");
}

std::optional<int> CompanyController::parseLimit(const httplib::Request &req) {
    if (!req.has_param("limit"))
        return std::nullopt;

    try {
        return std::stoi(req.get_param_value("limit"));
    } catch (const std::exception &) {
        return std::nullopt;
    }
}

std::vector<std::string> CompanyController::parseFacts(const httplib::Request &req) {
    if (!req.has_param("facts"))
        return {};

    std::vector<std::string> result;
    std::stringstream factsString(req.get_param_value("facts"));
    std::string fact;
    while (std::getline(factsString, fact, ',')) {
        fact.erase(0, fact.find_first_not_of(" \t"));
        fact.erase(fact.find_last_not_of(" \t") + 1);
        if (!fact.empty()) {
            result.push_back(fact);
        }
    }
    return result;
}

} // namespace controller
