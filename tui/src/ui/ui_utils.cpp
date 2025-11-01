#include "ui/ui_utils.h"
#include <ncurses.h>

// TODO: Refactor needed... This is not pretty
void UiUtils::renderHorizontalBarChart(WINDOW *win, const std::vector<DataPoint> &points, int startY, int startX) {
    if (points.empty())
        return;

    auto [minIt, maxIt] =
        std::minmax_element(points.begin(), points.end(), [](auto &a, auto &b) { return a.value < b.value; });
    double minValue = minIt->value;
    double maxValue = maxIt->value;

    // Avoid division by zero
    if (maxValue == minValue)
        return;

    int totalWidth = getmaxx(win) * 0.65;
    int zeroPos = 0;
    if (minValue >= 0) {
        zeroPos = startX;
    } else if (maxValue <= 0) {
        zeroPos = startX + totalWidth;
    } else {
        zeroPos = startX + static_cast<int>((0 - minValue) / (maxValue - minValue) * totalWidth);
    }

    for (size_t y = 0; y < points.size(); ++y) {
        const auto &p = points[y];
        std::string label = p.timePeriod + " | ";
        mvwprintw(win, startY + y, startX, "%s", label.c_str());

        int xPos = startX + static_cast<int>((p.value - minValue) / (maxValue - minValue) * totalWidth);
        if (p.value >= 0) {
            int barLength = std::max(1, xPos - zeroPos);
            for (int x = 0; x < barLength; ++x)
                mvwprintw(win, startY + y, label.size() + zeroPos + x, "\u25A0");
        } else {
            int barLength = std::max(1, xPos - zeroPos);
            for (int x = 0; x < barLength; ++x)
                mvwprintw(win, startY + y, label.size() + zeroPos - x, "\u25A0");
        }
        mvwprintw(win, startY + y, startX + totalWidth + 6, "| %s (%+5.1f%%)",
                  UiUtils::abbreviateNumber(p.value).c_str(), p.percentageChange);
    }

    // Draw the zero line
    if (minValue <= 0) {
        mvwvline(win, startY - 1, zeroPos, ACS_VLINE, points.size() + 1);
    }
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
