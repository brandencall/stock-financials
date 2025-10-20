#include "repositories/financial_fact_repository.h"
#include "models/company_record.h"

namespace db::repository {

FinancialFactRepository::FinancialFactRepository(Database &db) : db_(db) {}

void FinancialFactRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE IF NOT EXISTS financial_facts (
          filingId INTEGER NOT NULL,
          tag TEXT NOT NULL,
          start_date TEXT,
          end_date TEXT NOT NULL,
          value REAL,
          unit TEXT,
          source_tag TEXT,
          PRIMARY KEY (filingId, source_tag, start_date, end_date, unit),
          FOREIGN KEY (filingId) REFERENCES filings(filingId)
        )
    )";
}

void FinancialFactRepository::insert(const db::model::CompanyRecord &record) {
    try {
        db_.get() << R"(
            INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        )" << record.filingId
                  << record.friendlyTag << record.start << record.end << record.val << record.unit << record.realTag;
    } catch (const sqlite::errors::constraint_primarykey &e) {
        std::cerr << "Primary key constraint failed on 'FinancialFact' table.\n";
        std::cerr << "Filing Id: " << record.filingId << '\n';
        std::cerr << "Source Tag: " << record.realTag << '\n';
        std::cerr << "Start Date: " << record.start << '\n';
        std::cerr << "End Date: " << record.end << '\n';
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void FinancialFactRepository::insert(const db::model::FinancialFact &fact) {
    db_.get() << R"(
        INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )" << fact.filingId
              << fact.tag << fact.startDate << fact.endDate << fact.value << fact.unit << fact.sourceTag;
}

} // namespace db::repository
