#include "repositories/financial_fact_repository.h"
#include "models/company_record.h"
#include "models/financial_fact.h"

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

void FinancialFactRepository::upsert(const db::model::CompanyRecord &record) {
    db_.get() << R"(
         INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
         VALUES (?, ?, ?, ?, ?, ?, ?)
         ON CONFLICT(filingId, source_tag, start_date, end_date, unit)
         DO UPDATE SET
             tag = excluded.tag,
             value = excluded.value
         WHERE
             tag != excluded.tag OR
             value != excluded.value
        )" << record.filingId
              << record.friendlyTag << record.start << record.end << record.val << record.unit << record.realTag;
}

void FinancialFactRepository::upsert(const db::model::FinancialFact &fact) {
    db_.get() << R"(
            INSERT INTO financial_facts (filingId, tag, start_date, end_date, value, unit, source_tag)
            VALUES (?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT(filingId, source_tag, start_date, end_date, unit)
            DO UPDATE SET
                tag = excluded.tag,
                value = excluded.value
            WHERE
                tag != excluded.tag OR
                value != excluded.value
    )" << fact.filingId
              << fact.tag << fact.startDate << fact.endDate << fact.value << fact.unit << fact.sourceTag;
}

std::vector<model::FinancialFact> FinancialFactRepository::getByFilingId(int filingId) {
    std::vector<db::model::FinancialFact> result;
    db_.get() << R"(
        SELECT tag, start_date, end_date, value, unit, source_tag 
        FROM financial_facts 
        WHERE filingId = (?)
    )" << filingId >>
        [&](std::string tag, std::string startDate, std::string endDate, double value, std::string unit,
            std::string sourceTag) {
            model::FinancialFact fact;
            fact.filingId = filingId;
            fact.tag = std::move(tag);
            fact.startDate = std::move(startDate);
            fact.endDate = std::move(endDate);
            fact.value = value;
            fact.unit = std::move(unit);
            fact.sourceTag = std::move(sourceTag);
            result.push_back(fact);
        };
    return result;
}

std::vector<model::FinancialFact> FinancialFactRepository::getAllFilteredByFilingId(int filingId, std::string period) {
    std::vector<db::model::FinancialFact> result;
    // The only difference between annual and quarterly is the way we sort the start_date
    std::string sortOrder = (period == "annual") ? "ASC" : "DESC";
    std::string sql = R"(
        SELECT tag, start_date, end_date, value, unit, source_tag
        FROM (
            SELECT *,
                   ROW_NUMBER() OVER (
                       PARTITION BY tag
                       ORDER BY end_date DESC, start_date )" +
                      sortOrder + R"(
                   ) AS rn
            FROM financial_facts
            WHERE filingId = ? 
        ) t
        WHERE rn = 1
    )";
    db_.get() << sql << filingId >> [&](std::string tag, std::string startDate, std::string endDate, double value,
                                        std::string unit, std::string sourceTag) {
        model::FinancialFact fact;
        fact.filingId = filingId;
        fact.tag = std::move(tag);
        fact.startDate = std::move(startDate);
        fact.endDate = std::move(endDate);
        fact.value = value;
        fact.unit = std::move(unit);
        fact.sourceTag = std::move(sourceTag);
        result.push_back(fact);
    };
    return result;
}

std::vector<model::FinancialFact>
FinancialFactRepository::getFilteredByFilingIdAndFacts(int filingId, std::string period,
                                                       std::vector<std::string> facts) {
    std::vector<db::model::FinancialFact> result;

    std::string sortOrder = (period == "annual") ? "ASC" : "DESC";
    std::string sql = R"(
        SELECT tag, start_date, end_date, value, unit, source_tag
        FROM (
            SELECT *,
                   ROW_NUMBER() OVER (
                       PARTITION BY tag
                       ORDER BY end_date DESC, start_date )" +
                      sortOrder + R"(
                   ) AS rn
            FROM financial_facts
            WHERE filingId = ? AND tag IN (
    )";
    // Inserting ? placholders for the filtering
    for (size_t i = 0; i < facts.size(); ++i) {
        sql += (i == 0 ? "?" : ",?");
    }
    sql += R"(
            )
        ) t
        WHERE rn = 1
    )";

    auto stmt = (db_.get() << sql);
    stmt << filingId;

    for (const auto &fact : facts) {
        stmt << fact;
    }

    stmt >> [&](std::string tag, std::string startDate, std::string endDate, double value, std::string unit,
                std::string sourceTag) {
        model::FinancialFact fact;
        fact.filingId = filingId;
        fact.tag = std::move(tag);
        fact.startDate = std::move(startDate);
        fact.endDate = std::move(endDate);
        fact.value = value;
        fact.unit = std::move(unit);
        fact.sourceTag = std::move(sourceTag);
        result.push_back(std::move(fact));
    };

    return result;
}

} // namespace db::repository
