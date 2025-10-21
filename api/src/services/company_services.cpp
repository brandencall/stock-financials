#include "services/company_service.h"
#include <iostream>

namespace service {

CompanyService::CompanyService(db::repository::CompanyRepository companyRepo) : companyRepo(companyRepo) {};

std::vector<db::model::Company> CompanyService::getAllCompanies() {
    std::vector<db::model::Company> companies = companyRepo.getAll();
    return companies;
}

std::optional<db::model::Company> CompanyService::getCompanyByCIK(const std::string &cik) {
    return companyRepo.getCompanyByCIK(cik);
}

} // namespace service
