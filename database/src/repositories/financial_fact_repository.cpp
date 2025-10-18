#include "repositories/financial_fact_repository.h"
#include "models/company_record.h"

FinancialFactRepository::FinancialFactRepository(Database &db) : db_(db) {}

void FinancialFactRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE financial_facts (
          filingId INTEGER NOT NULL,
          tag TEXT NOT NULL,
          start_date TEXT,
          end_date TEXT NOT NULL,
          value REAL,
          unit TEXT,
          source_tag TEXT,
          PRIMARY KEY (filingId, tag, start_date, end_date),
          FOREIGN KEY (filingId) REFERENCES filings(filingId)
        )
    )";
}

void FinancialFactRepository::insert(const CompanyRecord &record) {
    db_.get() << R"(
        INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )" << record.filingId
              << record.friendlyTag << record.start << record.end << record.val << record.unit << record.realTag;
}

void FinancialFactRepository::insert(const FinancialFact &fact) {
    db_.get() << R"(
        INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )" << fact.filingId
              << fact.tag << fact.startDate << fact.endDate << fact.value << fact.unit << fact.sourceTag;
}
