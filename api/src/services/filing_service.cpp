#include "services/filing_service.h"

namespace service {

FilingService::FilingService(db::repository::FilingRepository filingRepo) : filingRepo(filingRepo) {};

std::vector<db::model::Filing> FilingService::getAllFilings(const std::string &cik) {
    return filingRepo.getFilingsForCIK(cik);
}

} // namespace service
