#pragma once
#include "../database.h"
#include "../models/filing.h"
#include "../models/company_record.h"
#include <optional>
#include <vector>

class FilingRepository {
  public:
    explicit FilingRepository(Database &db);
    void createTable();
    int insert(const CompanyRecord &record);
    int insert(const Filing &filing);
    std::optional<int> getFileIdByAccession(std::string accession);

  private:
    Database &db_;
};
