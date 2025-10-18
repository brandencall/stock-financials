#pragma once
#include "../database.h"
#include "../models/company_record.h"
#include "../models/financial_fact.h"
#include <vector>

class FinancialFactRepository {
  public:
    explicit FinancialFactRepository(Database &db);
    void createTable();
    void insert(const CompanyRecord &record);
    void insert(const FinancialFact &financialFact);

  private:
    Database &db_;
};
