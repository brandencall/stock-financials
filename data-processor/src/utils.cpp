#include "utils.h"
#include <fstream>

std::string computeSHA256(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
        throw std::runtime_error("Failed to create EVP_MD_CTX");

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1)
        throw std::runtime_error("EVP_DigestInit_ex failed");

    std::vector<unsigned char> buffer(8192);
    while (file.good()) {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            if (EVP_DigestUpdate(ctx, buffer.data(), bytesRead) != 1)
                throw std::runtime_error("EVP_DigestUpdate failed");
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;

    if (EVP_DigestFinal_ex(ctx, hash, &length) != 1)
        throw std::runtime_error("EVP_DigestFinal_ex failed");

    EVP_MD_CTX_free(ctx); // clean up

    std::ostringstream oss;
    for (unsigned int i = 0; i < length; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return oss.str();
}
