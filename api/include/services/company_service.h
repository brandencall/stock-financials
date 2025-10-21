#pragma once

#include "models/company.h"
#include "repositories/company_repository.h"
#include <optional>
#include <vector>

namespace service {

class CompanyService {
  public:
    CompanyService(db::repository::CompanyRepository companyRepo);

    std::vector<db::model::Company> getAllCompanies();
    std::optional<db::model::Company> getCompanyByCIK(const std::string &cik);

  private:
    db::repository::CompanyRepository companyRepo;
};
} // namespace service
