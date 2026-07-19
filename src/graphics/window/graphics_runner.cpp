#include "../../../include/graphics/window/graphics_runner.h"
#include "../../../include/engine/GameEngine.h"
#include "../../../include/graphics/window/canvas.h"
#include "../../../include/input/controller.h"
#include <opencv2/opencv.hpp>

GraphicsRunner::GraphicsRunner(GameEngine& engine, Canvas& canvas, Controller& controller, std::string windowName)
    : engine(engine), canvas(canvas), controller(controller),
      windowName(std::move(windowName)), quitRequested(false) {
    cv::namedWindow(this->windowName, cv::WINDOW_NORMAL | cv::WINDOW_FREERATIO);
    cv::setMouseCallback(this->windowName, &GraphicsRunner::mouseTrampoline, this);
    waitObserverId = engine.addWaitObserver([this](int elapsedMs) { onWait(elapsedMs); });
}

GraphicsRunner::~GraphicsRunner() {
    engine.removeWaitObserver(waitObserverId);
}

void GraphicsRunner::mouseTrampoline(int event, int x, int y, int flags, void* userdata) {
    static_cast<GraphicsRunner*>(userdata)->onMouse(event, x, y);
}

void GraphicsRunner::onMouse(int event, int x, int y) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        controller.handleInput(x, y);
    }
}

void GraphicsRunner::onWait(int elapsedMs) {
    cv::Rect windowRect = cv::getWindowImageRect(windowName);
    if (windowRect.width > 0 && windowRect.height > 0) {
        const cv::Mat& composed = canvas.composeAll(windowRect.width, windowRect.height);
        cv::imshow(windowName, composed);
    }

    int key = cv::waitKey(1);
    if (key == 27 || key == 'q') {
        quitRequested = true;
    }
}
