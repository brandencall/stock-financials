#include "database.h"
#include "server.h"
#include <httplib.h>

int main() {
    // std::filesystem::path dataDir= "../data/stock_data.db";
    std::filesystem::path dataDir = "/var/lib/stockfinances";
    db::Database db(dataDir);
    Server server(db);
    server.registerControllers();
    int port = 8080;
    std::cout << "Api currently running on port: " << port << '\n';
    server.run(port);
    return 0;
}
