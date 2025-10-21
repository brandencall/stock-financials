#include "server.h"

Server::Server(db::Database &db)
    : db(db), companyRepo(db), companyService(companyRepo), companyController(companyService) {}

void Server::registerControllers() {
    companyController.registerRoutes(server);
}

void Server::run(int port) { server.listen("0.0.0.0", port); }
