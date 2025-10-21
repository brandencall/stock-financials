#include "server.h"

Server::Server(db::Database &db) : db(db), companyRepo(db), companyService(companyRepo) {}

void Server::registerControllers() {
    controller::CompanyController companyController;
    companyController.registerRoutes(server, companyService);
}

void Server::run(int port) { server.listen("0.0.0.0", port); }
