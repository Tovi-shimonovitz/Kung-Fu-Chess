#include "../../include/graphics/graphics_runner.h"
#include <opencv2/opencv.hpp>

GraphicsRunner::GraphicsRunner(GameEngine& engine, BoardRenderer& renderer, std::string windowName)
    : engine(engine), renderer(renderer), windowName(std::move(windowName)), quitRequested(false) {
    cv::namedWindow(this->windowName);
    engine.addWaitObserver([this](int elapsedMs) { onWait(elapsedMs); });
}

void GraphicsRunner::onWait(int elapsedMs) {
    GameSnapshot snapshot = engine.getSnapshot();
    Img frame = renderer.render(snapshot, elapsedMs);
    cv::imshow(windowName, frame.get_mat());

    int key = cv::waitKey(1);
    if (key == 27 || key == 'q') { 
        quitRequested = true;
    }
}
