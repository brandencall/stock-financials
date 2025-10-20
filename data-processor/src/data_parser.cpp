#include "data_parser.h"
#include <fstream>

DataParser::DataParser(std::unordered_map<std::string, std::string> tagMap,
                       db::repository::FilingRepository &filingRepo, db::repository::FinancialFactRepository &factRepo)
    : tagMap(tagMap), filingRepo(filingRepo), factRepo(factRepo) {}

void DataParser::parseAndInsertData(const std::string &filename) {
    std::ifstream file(filename);
    json data = json::parse(file);
    std::string cik = std::to_string(data["cik"].get<long long>());
    std::unordered_map<std::string, int> filing_map;

    for (const auto &[taxonomy, tags] : data["facts"].items()) {
        for (auto &[tag, tagData] : tags.items()) {
            if (!tagData.contains("units"))
                continue;

            if (tagMap.find(tag) != tagMap.end()) {
                std::cout << "Tag: " << tag << '\n';
                db::model::CompanyRecord record;
                record.cik = cik;
                record.realTag = tag;
                record.friendlyTag = tagMap[tag];
                parseAndInsertTagData(record, tagData["units"], filing_map);
            }
        }
    }
}

void DataParser::parseAndInsertTagData(db::model::CompanyRecord &record, const json &tagData,
                                       std::unordered_map<std::string, int> &filing_map) {
    for (const auto &[unit, entries] : tagData.items()) {
        record.unit = unit;
        for (const auto &entry : entries) {
            parseCompanyRecord(entry, record);
            if (filing_map.find(record.accession) != filing_map.end()) {
                record.filingId = filing_map[record.accession];
                factRepo.insert(record);
            } else {
                handleNonCachedAccession(record, filing_map);
            }
        }
    }
}

void DataParser::parseCompanyRecord(const json &entry, db::model::CompanyRecord &record) {
    record.start = entry.value("start", "");

    if (entry.contains("accn") && !entry["accn"].is_null())
        record.accession = entry["accn"].get<std::string>();

    if (entry.contains("end") && !entry["end"].is_null())
        record.end = entry["end"].get<std::string>();

    if (entry.contains("val") && !entry["val"].is_null())
        record.val = entry["val"].get<double>();

    if (entry.contains("fy") && !entry["fy"].is_null())
        record.fy = entry["fy"].get<int>();

    if (entry.contains("fp") && !entry["fp"].is_null())
        record.fp = entry["fp"].get<std::string>();

    if (entry.contains("form") && !entry["form"].is_null())
        record.form = entry["form"].get<std::string>();

    if (entry.contains("filed") && !entry["filed"].is_null())
        record.filed = entry["filed"].get<std::string>();
}

void DataParser::handleNonCachedAccession(db::model::CompanyRecord &record,
                                          std::unordered_map<std::string, int> &filing_map) {
    std::optional<int> filingId = filingRepo.getFileIdByAccession(record.accession);
    if (filingId != std::nullopt) {
        filing_map[record.accession] = filingId.value();
        record.filingId = filingId.value();
        factRepo.insert(record);
    } else {
        int newFilingId = filingRepo.insert(record);
        filing_map[record.accession] = newFilingId;
        record.filingId = newFilingId;
        factRepo.insert(record);
    }
}
