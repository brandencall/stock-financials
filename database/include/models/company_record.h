#pragma once
#include <string>

struct CompanyRecord {
    std::string cik;
    std::string realTag;
    std::string friendlyTag;
    std::string unit;
    std::string accession;
    std::string start;
    std::string end;
    double val;
    int fy;
    std::string fp;
    std::string form;
    std::string filed;
};
