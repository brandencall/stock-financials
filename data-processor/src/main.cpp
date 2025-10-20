#include "application.h"

int main() {
    std::filesystem::create_directories("../data");
    std::filesystem::path path = std::filesystem::absolute("../data") / "stock_data.db";
    Application app(path);
    app.run();
    return 0;
}
