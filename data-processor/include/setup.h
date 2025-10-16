#include "database.h"
#include "repositories/company_repository.h"
#include <filesystem>
#include <iostream>
#include "database.h"
#include "repositories/company_repository.h"
#include "get_data.h"

Database setup_db();
CompanyRepository setup_company_repo(Database &db);
std::filesystem::path setup_company_fact_data();
