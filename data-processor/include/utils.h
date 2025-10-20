#pragma once
#include "string"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <vector>

std::string computeSHA256(const std::string &filename);
