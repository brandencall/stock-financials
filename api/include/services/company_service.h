#pragma once

#include "models/company.h"
#include "repositories/company_repository.h"
#include <vector>

namespace service {

class CompanyService {
  public:
    CompanyService(db::repository::CompanyRepository companyRepo);

    std::vector<db::model::Company> getAllCompanies();

  private:
    db::repository::CompanyRepository companyRepo;
};
} // namespace service
