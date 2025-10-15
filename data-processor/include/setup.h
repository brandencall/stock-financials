#include "database.h"
#include "repositories/company_repository.h"
#include <filesystem>
#include <iostream>

Database setup_db();
CompanyRepository setup_company_repo(Database &db);

