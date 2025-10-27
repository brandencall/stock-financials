#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace db::model {

struct Filing {
    int filingId;
    std::string accession;
    std::string cik;
    std::string form;
    int fy;
    std::string fp;
    std::string filedDate;
};

inline void to_json(json &j, const Filing &f) {
    j = json{
        {"filingId", f.filingId},
        {"accession", f.accession},
        {"cik", f.cik},
        {"form", f.form},
        {"fy", f.fy},
        {"fp", f.fp},
        {"filedDate", f.filedDate},
    };
}

} // namespace db::model
