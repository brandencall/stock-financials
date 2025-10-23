#pragma once
#include "../database.h"
#include "../models/stock_price.h"
#include <vector>

namespace db::repository {

class StockPriceRepository {
  public:
    explicit StockPriceRepository(db::Database &db);
    void createTable();
    void upsert(const db::model::StockPrice &stockPrice);

  private:
    db::Database &db_;
};

} // namespace db::repository
