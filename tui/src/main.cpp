#include "api_client.h"
#include "application.h"
#include <ncurses.h>

int main() {
    
    ApiClient client;
    Application app(client);
    app.initUi();
    app.run();
    return 0;
}

