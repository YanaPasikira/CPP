#include <windows.h>
#include <aclapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

void fileInfo(const wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    LARGE_INTEGER size;
    GetFileSizeEx(h, &size);

    FILETIME ct, at, mt;
    GetFileTime(h, &ct, &at, &mt);

    PSID owner;
    PSECURITY_DESCRIPTOR sd;
    GetSecurityInfo(h, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
                    &owner, NULL, NULL, NULL, &sd);

    WCHAR name[256], domain[256];
    DWORD nSize = 256, dSize = 256;
    SID_NAME_USE use;
    LookupAccountSidW(NULL, owner, name, &nSize, domain, &dSize, &use);

    cout << "Size: " << size.QuadPart << endl;
    cout << "Attributes: " << attrs << endl;
    wcout << L"Owner: " << domain << L"\\" << name << endl;

    CloseHandle(h);
    LocalFree(sd);
}

void cRead(const char* in, const char* out) {
    FILE* fi = fopen(in, "rb");
    FILE* fo = fopen(out, "wb");
    char buf[8192];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), fi)) > 0)
        fwrite(buf, 1, r, fo);
    fclose(fi);
    fclose(fo);
}

void winRead(const wchar_t* in, const wchar_t* out) {
    HANDLE fi = CreateFileW(in, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    HANDLE fo = CreateFileW(out, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING, NULL);

    BYTE buf[4096];
    DWORD r, w;
    while (ReadFile(fi, buf, sizeof(buf), &r, NULL) && r)
        WriteFile(fo, buf, r, &w, NULL);

    CloseHandle(fi);
    CloseHandle(fo);
}

void asyncRead(const vector<wstring>& files) {
    vector<HANDLE> handles;
    vector<OVERLAPPED> ovs;
    vector<HANDLE> events;

    for (auto& f : files) {
        HANDLE h = CreateFileW(f.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        OVERLAPPED ov = {};
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        BYTE* buf = new BYTE[4096];
        ReadFile(h, buf, 4096, NULL, &ov);

        handles.push_back(h);
        ovs.push_back(ov);
        events.push_back(ov.hEvent);
    }

    WaitForMultipleObjects(events.size(), events.data(), TRUE, INFINITE);

    for (int i = 0; i < handles.size(); i++) {
        DWORD r;
        GetOverlappedResult(handles[i], &ovs[i], &r, FALSE);
        CloseHandle(handles[i]);
        CloseHandle(events[i]);
    }
}

int main() {
    wstring path = L"test.bin";
    fileInfo(path);

    LARGE_INTEGER t1, t2, f;
    QueryPerformanceFrequency(&f);

    QueryPerformanceCounter(&t1);
    cRead("test.bin", "c_copy.bin");
    QueryPerformanceCounter(&t2);
    cout << "C IO time: " << (double)(t2.QuadPart - t1.QuadPart) / f.QuadPart << endl;

    QueryPerformanceCounter(&t1);
    winRead(L"test.bin", L"win_copy.bin");
    QueryPerformanceCounter(&t2);
    cout << "WinAPI IO time: " << (double)(t2.QuadPart - t1.QuadPart) / f.QuadPart << endl;

    vector<wstring> files = {L"test.bin", L"win_copy.bin"};
    asyncRead(files);

    return 0;
}

