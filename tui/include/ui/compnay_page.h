#pragma once

#include "../models.h"
#include "page.h"
#include <vector>

class Application;

class CompanyPage : public Page {
  public:
    explicit CompanyPage(Application &app, Company &company);
    void render() override;
    void handleInput(int ch) override;

  private:
    Application &app;
    Company &company;
};
