#include "database.h"
#include "server.h"
#include <httplib.h>

int main() {
    std::filesystem::path file_path = "../data/stock_data.db";
    db::Database db(file_path);
    Server server(db);
    server.registerControllers();
    int port = 8080;
    std::cout << "Api currently running on port: " << port << '\n';
    server.run(port);
    return 0;
}
