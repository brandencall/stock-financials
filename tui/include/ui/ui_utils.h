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
    static void renderHorizontalBarChart(WINDOW *win, const std::vector<DataPoint> &points, float barRatio, int startY,
                                         int startX);
    static void renderCashBalanceBar(WINDOW *win, double cash, double debt, int startY, int startX);
    static std::string abbreviateNumber(double num);
    static int getCenterForString(WINDOW *win, const std::string &str);

  private:
    static int getZeroPosition(int startX, int totalWidth, double minValue, double maxValue);
    static void printHorizontalBar(WINDOW *win, int zeroPos, int endPos, int startX, int yPos, double value,
                                   int labelSize);
};
