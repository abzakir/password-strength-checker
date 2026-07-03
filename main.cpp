#include <windows.h>

#include <cwctype>
#include <cwchar>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kMinPasswordLength = 8;

struct PasswordAnalysis {
    bool hasLength = false;
    bool hasUppercase = false;
    bool hasLowercase = false;
    bool hasDigit = false;
    bool hasSpecialCharacter = false;
    int score = 0;
    std::wstring strengthLabel;
    std::wstring summaryText;
};

struct AppState {
    HWND passwordEdit = nullptr;
    HWND showPasswordCheck = nullptr;
    HWND scoreValue = nullptr;
    HWND strengthValue = nullptr;
    HWND reportBox = nullptr;
    HFONT titleFont = nullptr;
    HFONT bodyFont = nullptr;
    HFONT monoFont = nullptr;
};

bool containsUppercase(const std::wstring& password) {
    for (wchar_t ch : password) {
        if (std::iswupper(ch)) {
            return true;
        }
    }
    return false;
}

bool containsLowercase(const std::wstring& password) {
    for (wchar_t ch : password) {
        if (std::iswlower(ch)) {
            return true;
        }
    }
    return false;
}

bool containsDigit(const std::wstring& password) {
    for (wchar_t ch : password) {
        if (std::iswdigit(ch)) {
            return true;
        }
    }
    return false;
}

bool containsSpecialCharacter(const std::wstring& password) {
    for (wchar_t ch : password) {
        if (!std::iswalnum(ch)) {
            return true;
        }
    }
    return false;
}

std::wstring strengthLabelForScore(int score) {
    if (score >= 5) {
        return L"STRONG";
    }
    if (score >= 3) {
        return L"MEDIUM";
    }
    return L"WEAK";
}

std::vector<std::wstring> buildSuggestions(const PasswordAnalysis& analysis) {
    std::vector<std::wstring> suggestions;

    if (!analysis.hasLength) {
        suggestions.emplace_back(L"Use at least 8 characters.");
    }
    if (!analysis.hasUppercase) {
        suggestions.emplace_back(L"Add at least one uppercase letter.");
    }
    if (!analysis.hasLowercase) {
        suggestions.emplace_back(L"Add at least one lowercase letter.");
    }
    if (!analysis.hasDigit) {
        suggestions.emplace_back(L"Add at least one number.");
    }
    if (!analysis.hasSpecialCharacter) {
        suggestions.emplace_back(L"Add at least one special character.");
    }

    return suggestions;
}

PasswordAnalysis analyzePassword(const std::wstring& password) {
    PasswordAnalysis analysis;
    analysis.hasLength = password.length() >= kMinPasswordLength;
    analysis.hasUppercase = containsUppercase(password);
    analysis.hasLowercase = containsLowercase(password);
    analysis.hasDigit = containsDigit(password);
    analysis.hasSpecialCharacter = containsSpecialCharacter(password);

    analysis.score = static_cast<int>(analysis.hasLength) +
                     static_cast<int>(analysis.hasUppercase) +
                     static_cast<int>(analysis.hasLowercase) +
                     static_cast<int>(analysis.hasDigit) +
                     static_cast<int>(analysis.hasSpecialCharacter);
    analysis.strengthLabel = strengthLabelForScore(analysis.score);

    std::wostringstream report;
    report << L"Password Analysis\r\n";
    report << L"-----------------\r\n";
    report << L"Length: " << (analysis.hasLength ? L"Good" : L"Poor") << L"\r\n";
    report << L"Uppercase: " << (analysis.hasUppercase ? L"Yes" : L"No") << L"\r\n";
    report << L"Lowercase: " << (analysis.hasLowercase ? L"Yes" : L"No") << L"\r\n";
    report << L"Number: " << (analysis.hasDigit ? L"Yes" : L"No") << L"\r\n";
    report << L"Special Character: " << (analysis.hasSpecialCharacter ? L"Yes" : L"No") << L"\r\n";
    report << L"\r\nScore: " << analysis.score << L"/5\r\n";
    report << L"Strength: " << analysis.strengthLabel << L"\r\n";

    const std::vector<std::wstring> suggestions = buildSuggestions(analysis);
    if (!suggestions.empty()) {
        report << L"\r\nSuggestions:\r\n";
        for (const std::wstring& suggestion : suggestions) {
            report << L"- " << suggestion << L"\r\n";
        }
    }

    analysis.summaryText = report.str();
    return analysis;
}

std::wstring getWindowTextString(HWND window) {
    const int length = GetWindowTextLengthW(window);
    std::wstring text(length + 1, L'\0');
    if (length > 0) {
        GetWindowTextW(window, text.data(), length + 1);
        text.resize(wcslen(text.c_str()));
    }
    return text;
}

void setControlFont(HWND control, HFONT font) {
    SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

void setAnalysisResult(const AppState& state, const PasswordAnalysis& analysis) {
    std::wostringstream scoreText;
    scoreText << analysis.score << L"/5";

    SetWindowTextW(state.scoreValue, scoreText.str().c_str());
    SetWindowTextW(state.strengthValue, analysis.strengthLabel.c_str());
    SetWindowTextW(state.reportBox, analysis.summaryText.c_str());
}

void updatePasswordVisibility(const AppState& state) {
    const bool showPassword = SendMessageW(state.showPasswordCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
    LONG_PTR style = GetWindowLongPtrW(state.passwordEdit, GWL_STYLE);

    if (showPassword) {
        style &= ~ES_PASSWORD;
    } else {
        style |= ES_PASSWORD;
    }

    SetWindowLongPtrW(state.passwordEdit, GWL_STYLE, style);
    SetWindowPos(state.passwordEdit, nullptr, 0, 0, 0, 0,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void layoutControls(HWND window, const AppState& state) {
    RECT rect{};
    GetClientRect(window, &rect);

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int margin = 18;
    const int fieldWidth = width - (margin * 2);
    const int reportTop = 230;
    const int reportHeight = (height - reportTop - margin > 120) ? (height - reportTop - margin) : 120;

    MoveWindow(GetDlgItem(window, 101), margin, 16, fieldWidth, 34, TRUE);
    MoveWindow(GetDlgItem(window, 102), margin, 58, fieldWidth, 22, TRUE);
    MoveWindow(state.passwordEdit, margin, 90, fieldWidth, 28, TRUE);
    MoveWindow(state.showPasswordCheck, margin, 126, 160, 22, TRUE);
    MoveWindow(GetDlgItem(window, 103), margin, 158, 120, 30, TRUE);
    MoveWindow(GetDlgItem(window, 104), margin + 130, 158, 120, 30, TRUE);
    MoveWindow(GetDlgItem(window, 105), width - 138, 156, 120, 34, TRUE);
    MoveWindow(state.reportBox, margin, reportTop, fieldWidth, reportHeight, TRUE);
}

void analyzeAndDisplay(HWND window) {
    AppState* state = reinterpret_cast<AppState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (!state) {
        return;
    }

    const std::wstring password = getWindowTextString(state->passwordEdit);
    const PasswordAnalysis analysis = analyzePassword(password);
    setAnalysisResult(*state, analysis);
}

LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    AppState* state = reinterpret_cast<AppState*>(GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        auto* createdState = new AppState();
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createdState));

        createdState->titleFont = CreateFontW(
            24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        createdState->bodyFont = CreateFontW(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        createdState->monoFont = CreateFontW(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Consolas");

        CreateWindowW(L"STATIC", L"Password Strength Checker", WS_CHILD | WS_VISIBLE | SS_LEFT,
                      0, 0, 0, 0, window, reinterpret_cast<HMENU>(101), GetModuleHandleW(nullptr), nullptr);
        CreateWindowW(L"STATIC",
                      L"Enter a password, then analyze its length, character mix, and overall strength.",
                      WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 0, 0, window,
                      reinterpret_cast<HMENU>(102), GetModuleHandleW(nullptr), nullptr);

        createdState->passwordEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD,
            0, 0, 0, 0, window, reinterpret_cast<HMENU>(201), GetModuleHandleW(nullptr), nullptr);
        createdState->showPasswordCheck = CreateWindowW(
            L"BUTTON", L"Show password", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            0, 0, 0, 0, window, reinterpret_cast<HMENU>(202), GetModuleHandleW(nullptr), nullptr);
        CreateWindowW(L"STATIC", L"Score", WS_CHILD | WS_VISIBLE | SS_LEFT,
                      0, 0, 0, 0, window, reinterpret_cast<HMENU>(103), GetModuleHandleW(nullptr), nullptr);
        createdState->scoreValue = CreateWindowW(L"STATIC", L"0/5", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                                0, 0, 0, 0, window, reinterpret_cast<HMENU>(203),
                                                GetModuleHandleW(nullptr), nullptr);
        CreateWindowW(L"STATIC", L"Strength", WS_CHILD | WS_VISIBLE | SS_LEFT,
                      0, 0, 0, 0, window, reinterpret_cast<HMENU>(104), GetModuleHandleW(nullptr), nullptr);
        createdState->strengthValue = CreateWindowW(L"STATIC", L"WEAK", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                                    0, 0, 0, 0, window, reinterpret_cast<HMENU>(204),
                                                    GetModuleHandleW(nullptr), nullptr);
        CreateWindowW(L"BUTTON", L"Analyze", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      0, 0, 0, 0, window, reinterpret_cast<HMENU>(105), GetModuleHandleW(nullptr), nullptr);
        createdState->reportBox = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY |
            ES_AUTOVSCROLL | WS_VSCROLL,
            0, 0, 0, 0, window, reinterpret_cast<HMENU>(206), GetModuleHandleW(nullptr), nullptr);

        setControlFont(GetDlgItem(window, 101), createdState->titleFont);
        setControlFont(GetDlgItem(window, 102), createdState->bodyFont);
        setControlFont(createdState->passwordEdit, createdState->bodyFont);
        setControlFont(createdState->showPasswordCheck, createdState->bodyFont);
        setControlFont(GetDlgItem(window, 103), createdState->bodyFont);
        setControlFont(createdState->scoreValue, createdState->bodyFont);
        setControlFont(GetDlgItem(window, 104), createdState->bodyFont);
        setControlFont(createdState->strengthValue, createdState->bodyFont);
        setControlFont(GetDlgItem(window, 105), createdState->bodyFont);
        setControlFont(createdState->reportBox, createdState->monoFont);

        SetFocus(createdState->passwordEdit);
        layoutControls(window, *createdState);
        return 0;
    }

    case WM_SIZE:
        if (state) {
            layoutControls(window, *state);
        }
        return 0;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            switch (LOWORD(wParam)) {
            case 105:
                analyzeAndDisplay(window);
                return 0;
            case 202:
                if (state) {
                    updatePasswordVisibility(*state);
                }
                return 0;
            default:
                break;
            }
        }
        return 0;

    case WM_SETFOCUS:
        if (state && state->passwordEdit) {
            SetFocus(state->passwordEdit);
        }
        return 0;

    case WM_DESTROY:
        if (state) {
            DeleteObject(state->titleFont);
            DeleteObject(state->bodyFont);
            DeleteObject(state->monoFont);
            delete state;
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
    const wchar_t className[] = L"PasswordStrengthCheckerWindow";

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = className;

    if (!RegisterClassExW(&windowClass)) {
        return 1;
    }

    HWND window = CreateWindowExW(
        0, className, L"Password Strength Checker",
        WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 720, 520,
        nullptr, nullptr, instance, nullptr);

    if (!window) {
        return 1;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR, int showCommand) {
    return wWinMain(instance, previousInstance, nullptr, showCommand);
}
