#pragma once

#include "models/filing.h"
#include "repositories/filing_repository.h"
#include <vector>

namespace service {

class FilingService{
  public:
    FilingService(db::repository::FilingRepository filingRepo);

    std::vector<db::model::Filing> getAllFilings(const std::string &cik);

  private:
    db::repository::FilingRepository filingRepo;
};
} // namespace service
