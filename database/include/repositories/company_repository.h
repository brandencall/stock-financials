#pragma once
#include "../models/company.h"
#include "../database.h"
#include <vector>

class CompanyRepository {
public:
    explicit CompanyRepository(Database& db);
    void createTable();
    void upsert(const Company& company);
    std::vector<Company> getAll();

    private:
    Database& db_;
};
