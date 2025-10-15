#include "repositories/company_repository.h"
#include <string>
#include <utility>

CompanyRepository::CompanyRepository(Database &db) : db_(db) {}

void CompanyRepository::createTable() {
    db_.get() << "CREATE TABLE IF NOT EXISTS companies ("
                 "cik TEXT PRIMARY KEY,"
                 "ticker TEXT,"
                 "title TEXT);";
}

void CompanyRepository::upsert(const Company &company) {
    db_.get() << R"(
        INSERT INTO companies (cik, ticker, title)
        VALUES (?, ?, ?)
        ON CONFLICT(cik) DO UPDATE SET
            ticker = excluded.ticker,
            title = excluded.title
        WHERE ticker != excluded.ticker OR title != excluded.title
    )" << company.cik
              << company.ticker << company.title;
}

std::vector<Company> CompanyRepository::getAll() {
    std::vector<Company> result;
    db_.get() << "SELECT cik, ticker, title FROM companies" >>
        [&](std::string cik, std::string ticker, std::string title) {
            result.push_back(Company{std::move(cik), std::move(ticker), std::move(title)});
        };
    return result;
}
