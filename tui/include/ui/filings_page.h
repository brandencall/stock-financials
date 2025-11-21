#pragma once

#include "../models.h"
#include "page.h"
#include <ncurses.h>

class Application;

class FilingsPage : public Page {
  public:
    explicit FilingsPage(Application &app, const Company &company);

    void render() override;
    void handleInput(int ch) override;

  private:
    Application &app;
    Company company;
};
