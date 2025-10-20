#include "repositories/company_facts_metadata.h"
#include "models/company_fact_metadata.h"

namespace db::repository {

CompanyFactsMetadataRepository::CompanyFactsMetadataRepository(Database &db) : db_(db) {}

void CompanyFactsMetadataRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE IF NOT EXISTS company_facts_metadata (
            cik TEXT PRIMARY KEY,
            hash TEXT,
            FOREIGN KEY (cik) REFERENCES companies(cik)
        )
    )";
}

void CompanyFactsMetadataRepository::upsert(std::string cik, std::string hash) {
    db_.get() << R"(
        INSERT INTO company_facts_metadata (cik, hash)
        VALUES (?, ?)
        ON CONFLICT(cik) DO UPDATE SET
        hash = excluded.hash
        WHERE hash != excluded.hash
    )" << cik << hash;
}

void CompanyFactsMetadataRepository::upsert(model::CompanyFactMetadata metadata) {
    db_.get() << R"(
        INSERT INTO company_facts_metadata (cik, hash)
        VALUES (?, ?)
        ON CONFLICT(cik) DO UPDATE SET
        hash = excluded.hash
        WHERE hash != excluded.hash
    )" << metadata.cik
              << metadata.hash;
}

std::optional<std::string> CompanyFactsMetadataRepository::getHashByCIK(const std::string &cik) {
    std::optional<std::string> result = std::nullopt;
    db_.get() << "SELECT hash FROM company_facts_metadata WHERE cik = (?)" << cik >>
        [&](std::string hash) { result = hash; };
    return result;
}
} // namespace db::repository
