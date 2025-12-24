#include <windows.h>
#include <iostream>
#include <vector>

int main() {
    const int PROCESS_COUNT = 2;
    const DWORD TIME_LIMIT = 10000;

    std::vector<PROCESS_INFORMATION> processes;
    std::vector<STARTUPINFO> startups;

    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < PROCESS_COUNT; i++) {
        STARTUPINFO si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);

        wchar_t commandLine[] = L"notepad.exe";

        if (CreateProcess(
            NULL,
            commandLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            startups.push_back(si);
            processes.push_back(pi);
        }
    }

    for (auto& pi : processes) {
        DWORD result = WaitForSingleObject(pi.hProcess, TIME_LIMIT);

        if (result == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            std::cout << "Процес завершено по тайм-ауту\n";
        } else {
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            std::cout << "Процес завершився з кодом: " << exitCode << "\n";
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    SetEvent(hEvent);
    CloseHandle(hEvent);

    return 0;
}

