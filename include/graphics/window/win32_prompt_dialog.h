#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <vector>

struct PromptResult {
    std::string buttonClicked; // empty if the window was closed without clicking a button
    std::vector<std::string> values; // one per field, in the order given
};

class Win32PromptDialog {
public:
    Win32PromptDialog(std::string title, std::vector<std::string> fieldLabels, std::vector<std::string> buttonLabels);

    PromptResult show();

private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    std::string title_;
    std::vector<std::string> fieldLabels_;
    std::vector<std::string> buttonLabels_;

    std::vector<HWND> editHandles_;
    std::string clickedButton_;
    bool done_ = false;
};
