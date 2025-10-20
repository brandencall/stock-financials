#include "application.h"
#include "utils.h"

int main() {
    std::filesystem::path dbPath = getDataDirectory();
    Application app(dbPath);
    app.run();
    return 0;
}
