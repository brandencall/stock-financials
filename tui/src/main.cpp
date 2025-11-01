#include "api_client.h"
#include "application.h"
#include <ncurses.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");
    
    ApiClient client;
    Application app(client);
    app.initUi();
    app.run();
    return 0;
}

