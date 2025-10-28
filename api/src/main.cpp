#include "database.h"
#include "server.h"
#include <httplib.h>

int main() {
    // std::filesystem::path dataFile= "../data/stock_data.db";
    std::filesystem::path dataFile= "/var/lib/stockfinances/stock_data.db";
    db::Database db(dataFile);
    Server server(db);
    server.registerControllers();
    int port = 8080;
    std::cout << "Api currently running on port: " << port << '\n';
    server.run(port);
    return 0;
}
