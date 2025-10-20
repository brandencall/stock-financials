#include "repositories/filing_repository.h"

namespace db::repository {

FilingRepository::FilingRepository(Database &db) : db_(db) {}

void FilingRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE IF NOT EXISTS filings (
          filingId INTEGER PRIMARY KEY AUTOINCREMENT,
          accession TEXT UNIQUE NOT NULL,
          cik TEXT NOT NULL,
          form TEXT,
          fy INTEGER,
          fp TEXT,
          filed_date TEXT,
          FOREIGN KEY (cik) REFERENCES companies(cik)
        )
    )";
}

int FilingRepository::upsert(const db::model::CompanyRecord &record) {
    auto &db = db_.get();
    db << R"(
         INSERT INTO filings (accession, cik, form, fy, fp, filed_date)
         VALUES (?, ?, ?, ?, ?, ?)
         ON CONFLICT(accession) DO UPDATE SET 
             cik = excluded.cik,
             form = excluded.form,
             fy = excluded.fy,
             fp = excluded.fp,
             filed_date = excluded.filed_date
         WHERE
             cik != excluded.cik OR
             form != excluded.form OR
             fy != excluded.fy OR
             fp != excluded.fp OR
             filed_date != excluded.filed_date
            )"
       << record.accession << record.cik << record.form << record.fy << record.fp << record.filed;
    int filingId = 0;
    db << "SELECT last_insert_rowid()" >> filingId;
    return filingId;
}

int FilingRepository::upsert(const db::model::Filing &filing) {
    auto &db = db_.get();
    db << R"(
         INSERT INTO filings (accession, cik, form, fy, fp, filed_date)
         VALUES (?, ?, ?, ?, ?, ?)
         ON CONFLICT(accession) DO UPDATE SET 
             cik = excluded.cik,
             form = excluded.form,
             fy = excluded.fy,
             fp = excluded.fp,
             filed_date = excluded.filed_date
         WHERE
             cik != excluded.cik OR
             form != excluded.form OR
             fy != excluded.fy OR
             fp != excluded.fp OR
             filed_date != excluded.filed_date
    )" << filing.filingId
       << filing.accession << filing.cik << filing.form << filing.fy << filing.fp << filing.filed_date;
    int filingId = 0;
    db << "SELECT last_insert_rowid()" >> filingId;
    return filingId;
}

std::optional<int> FilingRepository::getFileIdByAccession(std::string accession) {
    std::optional<int> result;
    db_.get() << "SELECT filingId FROM filings WHERE accession = (?)" << accession >>
        [&](int filingId) { result = filingId; };
    return result;
}

} // namespace db::repository
