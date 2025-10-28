#include "database.h"
#include "server.h"
#include <filesystem>
#include <httplib.h>

std::filesystem::path getDataFile() {
    if (const char *env_p = std::getenv("STOCK_DB_PATH")) {
        return env_p; // Use path from env
    }
    // Default to local dev path if not set
    return "../data/stock_data.db";
}

int main() {
    // std::filesystem::path dataFile= "../data/stock_data.db";
    std::filesystem::path dataFile = "/var/lib/stockfinances/stock_data.db";
    db::Database db(dataFile);
    Server server(db);
    server.registerControllers();
    int port = 8080;
    std::cout << "Api currently running on port: " << port << '\n';
    server.run(port);
    return 0;
}
