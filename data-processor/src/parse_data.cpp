#include "parse_data.h"
#include "models/company_record.h"

DataParser::DataParser(CompanyRecordService &crService, std::unordered_map<std::string, std::string> tagMap)
    : crService(crService), tagMap(tagMap) {}

void DataParser::parseAndInsertData(std::filesystem::path path) {
    std::ifstream file(path);
    json data = json::parse(file);
    std::string cik = std::to_string(data["cik"].get<long long>());

    for (const auto &[taxonomy, tags] : data["facts"].items()) {
        std::cout << "Processing taxonomy: " << taxonomy << '\n';
        for (auto &[tag, tagData] : tags.items()) {
            if (!tagData.contains("units"))
                continue;

            if (tagMap.find(tag) != tagMap.end()) {
                std::cout << "Tag: " << tag << '\n';
                parseFinancialFacts(cik, tag, tagMap[tag], tagData["units"]);
            }
        }
    }
}

void printFact(CompanyRecord record) {
    std::cout << "RealTag: " << record.realTag << '\n';
    std::cout << record.friendlyTag << " (" << record.unit << "): " << record.accession << " " << record.val << " FY"
              << record.fy << " " << record.fp << " " << record.start << " - " << record.end << " " << record.filed
              << " " << record.form << '\n';
}

void DataParser::parseFinancialFacts(const std::string &cik, const std::string &tag, const std::string &friedlyTag,
                                     const json &tagData) {
    for (const auto &[unit, entries] : tagData.items()) {
        CompanyRecord record;
        record.cik = cik;
        record.realTag = tag;
        record.friendlyTag = friedlyTag;
        record.unit = unit;
        for (const auto &entry : entries) {
            parseCompanyRecord(entry, record);
            crService.insertCompanyRecord(record);
        }
    }
}

void DataParser::parseCompanyRecord(const json &entry, CompanyRecord &record) {
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
