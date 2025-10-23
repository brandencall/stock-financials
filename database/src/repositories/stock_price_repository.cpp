#include "repositories/stock_price_repository.h"
#include "models/stock_price.h"
#include <iostream>

namespace db::repository {

StockPriceRepository::StockPriceRepository(Database &db) : db_(db) {}

void StockPriceRepository::createTable() {
    db_.get() << R"(
        CREATE TABLE IF NOT EXISTS stock_prices (
          filingId INTEGER NOT NULL,
          date TEXT,
          currency TEXT,
          open REAL,
          high REAL,
          low REAL,
          close REAL,
          volume REAL,
          PRIMARY KEY (filingId),
          FOREIGN KEY (filingId) REFERENCES filings(filingId)
        )
    )";
}

void StockPriceRepository::upsert(const db::model::StockPrice &stockPrice) {
    db_.get() << R"(
         INSERT INTO stock_prices (filingId, date, currency, open, high, low, close, volume)
         VALUES (?, ?, ?, ?, ?, ?, ?, ?)
         ON CONFLICT(filingId) DO UPDATE SET
             date = excluded.date,
             currency = excluded.currency,
             open = excluded.open,
             high = excluded.high,
             low = excluded.low,
             close = excluded.close,
             volume = excluded.volume
         WHERE
             date != excluded.date OR
             currency != excluded.currency OR
             open != excluded.open OR
             high != excluded.high OR
             low != excluded.low OR
             close != excluded.close OR
             volume != excluded.volume
    )" << stockPrice.filingId
              << stockPrice.date << stockPrice.currency << stockPrice.open << stockPrice.high << stockPrice.low
              << stockPrice.close << stockPrice.volume;
}

} // namespace db::repository
