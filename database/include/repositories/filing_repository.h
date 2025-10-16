#pragma once
#include "../database.h"
#include "../models/filing.h"
#include <optional>
#include <vector>

class FilingRepository {
  public:
    explicit FilingRepository(Database &db);
    void createTable();
    void insert(const Filing &filing);
    std::optional<int> getFileIdByAccession(std::string accession);

  private:
    Database &db_;
};
