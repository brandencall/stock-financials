#include "application.h"

Application::Application(const std::string &dbPath)
    : curl(), db(dbPath), companyRepo(db), filingRepo(db), factRepo(db), metadataRepo(db), stockPriceRepo(db) {
    companyRepo.createTable();
    filingRepo.createTable();
    factRepo.createTable();
    metadataRepo.createTable();
    stockPriceRepo.createTable();
}

void Application::run() {
    processCompanyTickers();
    std::filesystem::path tmpDir = getDataDirectory() / "tmp";
    std::string bulkDataDir = initializeCompanyBulkData(tmpDir);
    processCompanyData(bulkDataDir);
    deleteDirectory(tmpDir);
}

void Application::processCompanyTickers() {
    std::vector<db::model::Company> companies = curl.getSecCompanyTickers();
    std::cout << "Inserting companies into the 'company' table!" << '\n';
    for (const auto &company : companies) {
        companyRepo.upsert(company);
    }
    std::cout << "Done inserting!" << '\n';
}

std::string Application::initializeCompanyBulkData(const std::filesystem::path &tmpDir) {
    std::filesystem::create_directories(tmpDir);
    std::filesystem::path file_path = std::filesystem::absolute(tmpDir) / "companyfacts.zip";
    std::filesystem::path extract_dir = std::filesystem::absolute(tmpDir) / "companyfacts";

    std::cout << "Getting bulk data" << '\n';
    curl.getSecBulkData(file_path);
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

void Application::processCompanyData(const std::filesystem::path &factsDir) {
    auto tagMap = buildTagMap();

    DataParser parser(tagMap, filingRepo, factRepo);
    for (const auto &filePath : std::filesystem::directory_iterator(factsDir)) {
        std::string cik = getCIK(filePath.path().filename());
        std::optional<db::model::Company> company = companyRepo.getCompanyByCIK(cik);
        if (company != std::nullopt) {
            std::cout << "Processing CIK: " << cik << '\n';
            std::string computedHash = computeSHA256(filePath.path());
            std::optional<std::string> storedHash = metadataRepo.getHashByCIK(cik);
            if (storedHash == std::nullopt || computedHash != storedHash) {
                metadataRepo.upsert(cik, computedHash);
                parser.parseAndInsertData(filePath.path(), cik);
                insertPriceData(cik);
                break;
            }
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

void Application::insertPriceData(const std::string &cik) {
    std::optional<db::model::Company> company = companyRepo.getCompanyByCIK(cik);
    std::vector<db::model::StockPrice> stockPrices = curl.getStockPriceData(company->ticker);
    std::vector<db::model::Filing> filings = filingRepo.getFilingsForCIK(cik);

    // Hash filed_date with filingId for easy look up.
    std::unordered_map<std::string, int> date_map;

    for (const auto &filing : filings) {
        date_map[filing.filed_date] = filing.filingId;
    }
    for (auto &price : stockPrices) {
        if (date_map.count(price.date)) {
            std::cout << "Inserting into price repo for date: " << price.date << '\n';
            price.filingId = date_map[price.date];
            stockPriceRepo.upsert(price);
        }
    }
}

std::string Application::getCIK(const std::string &cik) {
    int start = 0;
    int end = cik.find('.');
    std::string result;
    for (std::string::size_type i = 3; i < cik.size(); ++i) {
        if (cik[i] == '0')
            continue;
        start = i;
        break;
    }
    int cikLen = end - start;
    return cik.substr(start, cikLen);
}
