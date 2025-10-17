#include "database.h"
#include "get_data.h"
#include "repositories/company_repository.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

Database setup_db();
CompanyRepository setup_company_repo(Database &db);
std::filesystem::path setup_company_fact_data();
std::unordered_map<std::string, std::string> buildTagMap();
