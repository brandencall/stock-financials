#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace db::model {

struct CompanyFactMetadata {
    std::string cik;
    std::string hash;
};

} // namespace db::model
