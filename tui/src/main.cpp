#include "api_client.h"
#include "application.h"
#include <locale.h>
#include <ncurses.h>

int main() {
    setlocale(LC_ALL, "");

    ApiClient client;
    Application app(client);
    app.initUi();
    app.run();
    return 0;
}
