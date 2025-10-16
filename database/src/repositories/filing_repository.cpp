#include "repositories/filing_repository.h"

FilingRepository::FilingRepository(Database &db) : db_(db) {}

void FilingRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE filings (
          filingId INTEGER PRIMARY KEY AUTOINCREMENT,
          accession TEXT UNIQUE NOT NULL,
          cik TEXT NOT NULL,
          form TEXT,
          fy INTEGER,
          fp TEXT,
          filed_date TEXT,
          FOREIGN KEY (cik) REFERENCES companies(cik)
        );
    )";
}

void FilingRepository::insert(const Filing &filing) {
    db_.get() << R"(
        INSERT INTO filings (filingId, accession, cik, form, fy, fp, filed_date)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )" << filing.filingId
              << filing.accession << filing.cik << filing.form << filing.fy << filing.fp << filing.filed_date;
}

std::optional<int> FilingRepository::getFileIdByAccession(std::string accession) {
    std::optional<int> result;
    db_.get() << "SELECT filingId FROM filings WHERE accession = (?);" << accession >>
        [&](int filingId) { result = filingId; };
    return result;
}
