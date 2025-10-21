#include "database.h"
#include "server.h"
#include <httplib.h>

int main() {
    std::filesystem::path file_path = "../data/stock_data.db";
    db::Database db(file_path);
    Server server(db);
    server.registerControllers();
    server.run(8080);
    return 0;
}
