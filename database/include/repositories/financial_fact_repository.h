#pragma once
#include "../models/financial_fact.h"
#include "../database.h"
#include <vector>

class FinancialFactRepository {
public:
    explicit FinancialFactRepository(Database& db);
    void createTable();
    void insert(const FinancialFact& financialFact);

    private:
    Database& db_;
};
