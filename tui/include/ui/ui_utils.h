#pragma once
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <vector>

struct DataPoint {
    std::string timePeriod;
    double value;
    float percentageChange;
};

class UiUtils {
  public:
    static void renderHorizontalBarChart(WINDOW *win, const std::vector<DataPoint> &points, int startY, int startX);
    static std::string abbreviateNumber(double num);
    static int getCenterForString(WINDOW *win, const std::string &str);
};
