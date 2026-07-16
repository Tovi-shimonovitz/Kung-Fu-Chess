#include "../../include/graphics/graphics_runner.h"
#include "../../include/engine/GameEngine.h"
#include "../../include/graphics/board_renderer.h"
#include "../../include/graphics/board_frame_manager.h"
#include "../../include/input/controller.h"
#include <opencv2/opencv.hpp>

GraphicsRunner::GraphicsRunner(GameEngine& engine, BoardRenderer& renderer,
                                BoardFrameManager& frameManager, Controller& controller,
                                std::string windowName)
    : engine(engine), renderer(renderer), frameManager(frameManager),
      controller(controller), windowName(std::move(windowName)), quitRequested(false) {
    cv::namedWindow(this->windowName, cv::WINDOW_NORMAL);
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
        GameSnapshot snapshot = engine.getSnapshot();
        Img frame = renderer.render(snapshot, elapsedMs);
        Img canvas = frameManager.compose(frame, windowRect.width,
                    windowRect.height, snapshot.width, snapshot.height);
        cv::imshow(windowName, canvas.get_mat());
    }

    int key = cv::waitKey(1);
    if (key == 27 || key == 'q') {
        quitRequested = true;
    }
}
