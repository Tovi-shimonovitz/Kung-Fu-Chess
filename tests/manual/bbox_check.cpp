#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat board = cv::imread("third_party/images/board.png", cv::IMREAD_UNCHANGED);
    std::cout << "board size: " << board.cols << "x" << board.rows << " channels=" << board.channels() << "\n";

    // sample a horizontal strip near the top to find where square color changes happen
    int y = 5;
    cv::Vec3b prev = board.at<cv::Vec3b>(y, 0);
    std::cout << "color changes along row y=" << y << ": x=0";
    for (int x = 1; x < board.cols; ++x) {
        cv::Vec3b cur = board.at<cv::Vec3b>(y, x);
        if (cv::norm(cur - prev) > 30) {
            std::cout << " " << x;
            prev = cur;
        }
    }
    std::cout << "\n";

    int x = 5;
    cv::Vec3b prevV = board.at<cv::Vec3b>(0, x);
    std::cout << "color changes along col x=" << x << ": y=0";
    for (int yy = 1; yy < board.rows; ++yy) {
        cv::Vec3b cur = board.at<cv::Vec3b>(yy, x);
        if (cv::norm(cur - prevV) > 30) {
            std::cout << " " << yy;
            prevV = cur;
        }
    }
    std::cout << "\n";
    return 0;
}
