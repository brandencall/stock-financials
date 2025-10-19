#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace db::model {

struct Filing {
    int filingId;
    std::string accession;
    std::string cik;
    std::string form;
    int fy;
    std::string fp;
    std::string filed_date;
};

} // namespace db::model
