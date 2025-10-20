#include "application.h"
#include "utils.h"
#include <iostream>

Application::Application(const std::string &dbPath)
    : db(dbPath), companyRepo(db), filingRepo(db), factRepo(db), metadataRepo(db) {
    companyRepo.createTable();
    filingRepo.createTable();
    factRepo.createTable();
    metadataRepo.createTable();
}

void Application::run() {
    // processCompanyTickers();
    std::filesystem::path tmpDir = getDataDirectory() / "tmp";
    // std::string bulkDataDir = initializeCompanyBulkData(tmpDir);
    std::filesystem::path bulkDataDir = std::filesystem::absolute(tmpDir) / "companyfacts";
    processCompanyFacts(bulkDataDir);
    deleteDirectory(tmpDir);
}

void Application::processCompanyTickers() {
    std::vector<db::model::Company> companies = get_sec_company_tickers();
    std::cout << "Inserting companies into the 'company' table!" << '\n';
    for (const auto &company : companies) {
        companyRepo.upsert(company);
    }
    std::cout << "Done inserting!" << '\n';
}

std::string Application::initializeCompanyBulkData(std::filesystem::path tmpDir) {
    std::filesystem::create_directories(tmpDir);
    std::filesystem::path file_path = std::filesystem::absolute(tmpDir) / "companyfacts.zip";
    std::filesystem::path extract_dir = std::filesystem::absolute(tmpDir) / "companyfacts";

    std::cout << "Getting bulk data" << '\n';
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

void Application::processCompanyFacts(const std::filesystem::path &factsDir) {
    auto tagMap = buildTagMap();

    DataParser parser(tagMap, filingRepo, factRepo);
    int count = 0;
    for (const auto &filePath : std::filesystem::directory_iterator(factsDir)) {
        std::string cik = getCIK(filePath.path().filename());
        std::optional<db::model::Company> company = companyRepo.getCompanyByCIK(cik);
        if (company != std::nullopt) {
            std::cout << "Processing: " << count << '\n';
            std::cout << "cik: " << cik << '\n';
            std::string computedHash = computeSHA256(filePath.path());
            std::optional<std::string> storedHash = metadataRepo.getHashByCIK(cik);
            if (storedHash == std::nullopt || computedHash != storedHash) {
                metadataRepo.upsert(cik, computedHash);
                parser.parseAndInsertData(filePath.path());
            }
            if (count == 5)
                break;
            count++;
        }
    }
}

std::unordered_map<std::string, std::string> Application::buildTagMap() {
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

std::string Application::getCIK(std::string cik) {
    int start = 0;
    int end = cik.find('.');
    std::string result;
    for (int i = 3; i < cik.size(); ++i) {
        if (cik[i] == '0')
            continue;
        start = i;
        break;
    }
    int cikLen = end - start;
    return cik.substr(start, cikLen);
}
