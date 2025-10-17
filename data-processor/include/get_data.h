#pragma once
#include "models/company.h"
#include <cstddef>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

std::vector<Company> get_sec_company_tickers();
void get_sec_bulk_data(std::filesystem::path path);
