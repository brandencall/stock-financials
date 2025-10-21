#include "services/company_service.h"

namespace service {

CompanyService::CompanyService(db::repository::CompanyRepository companyRepo) : companyRepo(companyRepo) {};

std::vector<db::model::Company> CompanyService::getAllCompanies() { return companyRepo.getAll(); }

} // namespace service
