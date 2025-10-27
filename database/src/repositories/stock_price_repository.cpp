#include "repositories/stock_price_repository.h"
#include "models/stock_price.h"

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

model::StockPrice StockPriceRepository::getByFilingId(int filingId) {
    model::StockPrice stockPrice;
    stockPrice.filingId = filingId;
    db_.get() << R"(
        SELECT date, currency, open, high, low, close, volume
        FROM stock_prices
        WHERE filingId = (?)
    )" << filingId >>
        [&](std::string date, std::string currency, double open, double high, double low, double close, double volume) {
            stockPrice.date = std::move(date);
            stockPrice.currency = std::move(currency);
            stockPrice.open = open;
            stockPrice.high = high;
            stockPrice.low = low;
            stockPrice.close = close;
            stockPrice.volume = volume;
        };
    return stockPrice;
}

} // namespace db::repository
