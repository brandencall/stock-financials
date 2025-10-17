#include "services/company_record_service.h"
#include <iostream>

CompanyRecordService::CompanyRecordService(Database &db) : companyRepo(db), filingRepo(db), financialFactRepo(db) {}

void CompanyRecordService::insertCompanyRecord(const CompanyRecord &record) {
    std::cout << "Trying to insert record for: " << record.cik << ", " << record.accession << '\n';
}
