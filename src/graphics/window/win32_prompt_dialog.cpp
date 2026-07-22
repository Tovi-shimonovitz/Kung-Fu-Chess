#include "../../../include/graphics/window/win32_prompt_dialog.h"

namespace {
constexpr int MARGIN = 20;
constexpr int LABEL_WIDTH = 100;
constexpr int FIELD_WIDTH = 200;
constexpr int FIELD_HEIGHT = 24;
constexpr int ROW_SPACING = 34;
constexpr int BUTTON_WIDTH = 80;
constexpr int BUTTON_HEIGHT = 28;
constexpr int BUTTON_SPACING = 10;
const char* WINDOW_CLASS_NAME = "KungFuChessPromptDialog";
}

Win32PromptDialog::Win32PromptDialog(std::string title, std::vector<std::string> fieldLabels, std::vector<std::string> buttonLabels)
    : title_(std::move(title)), fieldLabels_(std::move(fieldLabels)), buttonLabels_(std::move(buttonLabels)) {}

LRESULT CALLBACK Win32PromptDialog::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32PromptDialog* self = nullptr;
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
        self = reinterpret_cast<Win32PromptDialog*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<Win32PromptDialog*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    if (self) return self->handleMessage(hwnd, msg, wParam, lParam);
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

LRESULT Win32PromptDialog::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            int y = MARGIN;
            for (size_t i = 0; i < fieldLabels_.size(); ++i) {
                CreateWindowExA(0, "STATIC", fieldLabels_[i].c_str(), WS_CHILD | WS_VISIBLE,
                    MARGIN, y, LABEL_WIDTH, FIELD_HEIGHT, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
                HWND edit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
                    MARGIN + LABEL_WIDTH, y, FIELD_WIDTH, FIELD_HEIGHT, hwnd,
                    reinterpret_cast<HMENU>(1000 + i), GetModuleHandle(nullptr), nullptr);
                editHandles_.push_back(edit);
                y += ROW_SPACING;
            }
            int totalButtonsWidth = static_cast<int>(buttonLabels_.size()) * BUTTON_WIDTH +
                static_cast<int>(buttonLabels_.size() - 1) * BUTTON_SPACING;
            int startX = MARGIN + LABEL_WIDTH + FIELD_WIDTH - totalButtonsWidth;
            for (size_t i = 0; i < buttonLabels_.size(); ++i) {
                CreateWindowExA(0, "BUTTON", buttonLabels_[i].c_str(),
                    WS_CHILD | WS_VISIBLE | (i == 0 ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
                    startX + static_cast<int>(i) * (BUTTON_WIDTH + BUTTON_SPACING), y,
                    BUTTON_WIDTH, BUTTON_HEIGHT, hwnd, reinterpret_cast<HMENU>(2000 + i), GetModuleHandle(nullptr), nullptr);
            }
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (HIWORD(wParam) == BN_CLICKED && id >= 2000) {
                clickedButton_ = buttonLabels_[id - 2000];
                done_ = true;
            }
            return 0;
        }
        case WM_CLOSE:
        case WM_DESTROY:
            done_ = true;
            return 0;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}

PromptResult Win32PromptDialog::show() {
    static bool registered = false;
    if (!registered) {
        WNDCLASSA wc{};
        wc.lpfnWndProc = &Win32PromptDialog::windowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
        RegisterClassA(&wc);
        registered = true;
    }

    int width = MARGIN * 2 + LABEL_WIDTH + FIELD_WIDTH;
    int height = MARGIN * 2 + static_cast<int>(fieldLabels_.size()) * ROW_SPACING + BUTTON_HEIGHT + 40;

    HWND hwnd = CreateWindowExA(0, WINDOW_CLASS_NAME, title_.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandle(nullptr), this);
    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (!done_ && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    PromptResult result;
    result.buttonClicked = clickedButton_;
    for (HWND editHandle : editHandles_) {
        char buffer[256] = {};
        GetWindowTextA(editHandle, buffer, sizeof(buffer));
        result.values.push_back(buffer);
    }
    DestroyWindow(hwnd);
    return result;
}
