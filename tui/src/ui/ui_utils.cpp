#include "ui/ui_utils.h"
#include <ncurses.h>

void UiUtils::renderHorizontalBarChart(WINDOW *win, const std::vector<DataPoint> &points, float barRatio, int startY,
                                       int startX) {
    if (points.empty())
        return;

    auto [minIt, maxIt] =
        std::minmax_element(points.begin(), points.end(), [](auto &a, auto &b) { return a.value < b.value; });
    double minValue = minIt->value;
    double maxValue = maxIt->value;

    // Avoid division by zero
    if (minValue == maxValue)
        return;

    int totalWidth = getmaxx(win) * barRatio;
    int zeroPos = getZeroPosition(startX, totalWidth, minValue, maxValue);
    int labelSize = points[0].timePeriod.size() + 3;
    zeroPos += labelSize;

    for (size_t y = 0; y < points.size(); ++y) {
        const auto &p = points[y];
        std::string label = p.timePeriod + " | ";
        mvwprintw(win, startY + y, startX, "%s", label.c_str());

        int endPos = startX + static_cast<int>((p.value - minValue) / (maxValue - minValue) * totalWidth);
        printHorizontalBar(win, zeroPos, endPos, startX, startY + y, p.value, label.size());

        mvwprintw(win, startY + y, startX + totalWidth + 6, "| %s (%+5.1f%%)",
                  UiUtils::abbreviateNumber(p.value).c_str(), p.percentageChange);
    }

    // Draw the zero line
    if (minValue <= 0) {
        mvwvline(win, startY, zeroPos, ACS_VLINE, points.size());
    }
}

int UiUtils::getZeroPosition(int startX, int totalWidth, double minValue, double maxValue) {
    int zeroPos = 0;
    if (minValue >= 0) {
        zeroPos = startX;
    } else if (maxValue <= 0) {
        zeroPos = startX + totalWidth;
    } else {
        zeroPos = startX + static_cast<int>((0 - minValue) / (maxValue - minValue) * totalWidth);
    }
    return zeroPos;
}

void UiUtils::printHorizontalBar(WINDOW *win, int zeroPos, int endPos, int startX, int yPos, double value,
                                 int labelSize) {
    int barLength = std::max(1, endPos - zeroPos);
    // No negative values in graph
    if (zeroPos == startX && value >= 0) {
        for (int x = 0; x < barLength; ++x)
            mvwprintw(win, yPos, labelSize + zeroPos + x, "\u25A0");
        // Graph contains negative values but current value is positive
    } else if (value >= 0) {
        for (int x = 0; x < barLength; ++x)
            mvwprintw(win, yPos, zeroPos + x + 1, "\u25A0");
        // Current value is negative
    } else {
        for (int x = zeroPos - 1; x > endPos + labelSize; --x)
            mvwprintw(win, yPos, x, "\u25A0");
    }
}

void UiUtils::renderCashBalanceBar(WINDOW *win, double cash, double debt, int startY, int startX) {
    int winWidth = getmaxx(win);
    // subtract 10 for cash and debt labels
    int barWidth = winWidth - 10;
    float ratioCash = cash / (cash + debt);
    float ratioDebt = debt / (cash + debt);
    int cashWidth = ratioCash * ((float)barWidth / 2);
    int debtWidth = ratioDebt * ((float)barWidth / 2);

    mvwprintw(win, startY, startX + 1, "Cash");
    int cashBarOffset = 6;

    for (int x = 0; x < barWidth / 2; ++x) {
        if (x < cashWidth) {
            mvwprintw(win, startY, startX + x + cashBarOffset, "\u25A0");
        } else {
            mvwprintw(win, startY, startX + x + cashBarOffset, "─");
        }
    }

    mvwprintw(win, startY, winWidth - 6, "Debt");
    int debtBarOffset = 8;
    for (int x = winWidth - debtBarOffset, j = 0; x > barWidth / 2; --x, ++j) {
        if (j < debtWidth) {
            mvwprintw(win, startY, x, "\u25A0");
        } else {
            mvwprintw(win, startY, x, "─");
        }
    }

    mvwprintw(win, startY, startX + (barWidth / 2) + cashBarOffset, "|");

    mvwprintw(win, startY + 1, startX + cashBarOffset, "%s", UiUtils::abbreviateNumber(cash).c_str());
    mvwprintw(win, startY + 1, winWidth - debtBarOffset, "%s", UiUtils::abbreviateNumber(debt).c_str());

    std::string netCash = "Net Cash: " + UiUtils::abbreviateNumber(cash - debt);
    mvwprintw(win, startY + 2, winWidth / 2 - (netCash.length() / 2), "%s", netCash.c_str());
}

std::string UiUtils::abbreviateNumber(double num) {
    double absNum = std::fabs(num);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (absNum >= 1'000'000'000'000.0) {
        oss << (num / 1'000'000'000'000.0) << " T";
    } else if (absNum >= 1'000'000'000.0) {
        oss << (num / 1'000'000'000.0) << " B";
    } else if (absNum >= 1'000'000.0) {
        oss << (num / 1'000'000.0) << " M";
    } else if (absNum >= 1'000.0) {
        oss << (num / 1'000.0) << " K";
    } else {
        oss << num;
    }
    std::string result = oss.str();

    if (result.size() > 3 && result.substr(result.size() - 3) == ".00") {
        result.erase(result.size() - 3);
    }

    return oss.str();
}

int UiUtils::getCenterForString(WINDOW *win, const std::string &str) { return (getmaxx(win) / 2) - (str.size() / 2); }
