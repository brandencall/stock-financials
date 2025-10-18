#include "setup.h"
#include <iostream>

Database setup_db() {
    std::filesystem::path newDir = "../data";
    std::filesystem::create_directories(newDir);
    std::filesystem::path path = std::filesystem::absolute(newDir) / "stock_data.db";
    return Database(path);
}

CompanyRepository setup_company_repo(Database &db) {
    CompanyRepository companies(db);
    companies.createTable();
    return companies;
}
FilingRepository setup_filing_repo(Database &db) {
    FilingRepository filing(db);
    filing.createTable();
    return filing;
}

FinancialFactRepository setup_fact_repo(Database &db) {
    FinancialFactRepository fact(db);
    fact.createTable();
    return fact;
}

std::filesystem::path setup_company_fact_data() {
    std::filesystem::path newDir = "../data";
    std::filesystem::create_directories(newDir);
    std::filesystem::path tmpDir = "../data/tmp";
    std::filesystem::create_directories(tmpDir);
    std::filesystem::path file_path = std::filesystem::absolute(tmpDir) / "companyfacts.zip";
    std::filesystem::path extract_dir = std::filesystem::absolute(tmpDir) / "companyfacts";

    get_sec_bulk_data(file_path);
    std::string cmd = "unzip -o " + file_path.string() + " -d " + extract_dir.string() + " > /dev/null";
    std::cout << "Running: " << cmd << std::endl;

    int result = std::system(cmd.c_str());

    if (result == 0) {
        std::cout << "Unzip successful to: " << extract_dir << std::endl;
    } else {
        std::cerr << "Unzip failed with code " << result << std::endl;
        extract_dir.clear();
    }
    return extract_dir;
}

std::unordered_map<std::string, std::string> buildTagMap() {
    std::ifstream ifs("financial_mapping.json");
    json data = json::parse(ifs);
    std::unordered_map<std::string, std::string> result;
    for (auto &[canonical, aliases] : data.items()) {
        for (const auto &alias : aliases) {
            result[alias.get<std::string>()] = canonical;
        }
    }
    return result;
}
