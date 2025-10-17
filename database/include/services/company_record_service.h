#pragma once
#include "../database.h"
#include "../models/company_record.h"
#include "../repositories/company_repository.h"
#include "../repositories/filing_repository.h"
#include "../repositories/financial_fact_repository.h"

class CompanyRecordService {
  public:
    explicit CompanyRecordService(Database &db);

    void insertCompanyRecord(const CompanyRecord &record);

  private:
    CompanyRepository companyRepo;
    FilingRepository filingRepo;
    FinancialFactRepository financialFactRepo;
};
