#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <iostream>
#include <string>
#include <sstream>
#include <json/json.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct Data {
    int id;
    string text;
};

string serialize(const Data& d) {
    Json::Value r;
    r["id"] = d.id;
    r["text"] = d.text;
    Json::StreamWriterBuilder w;
    return Json::writeString(w, r);
}

Data deserialize(const string& s) {
    Json::Value r;
    Json::CharReaderBuilder b;
    string err;
    stringstream ss(s);
    Json::parseFromStream(b, ss, &r, &err);
    return { r["id"].asInt(), r["text"].asString() };
}

DWORD WINAPI ClientThread(LPVOID lp) {
    SOCKET c = (SOCKET)lp;
    char buf[1024];
    int n = recv(c, buf, sizeof(buf), 0);
    if (n > 0) {
        Data d = deserialize(string(buf, n));
        string resp = serialize({ d.id, "ACK: " + d.text });
        send(c, resp.c_str(), resp.size(), 0);
    }
    closesocket(c);
    return 0;
}

void tcpServer() {
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(54000);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (sockaddr*)&a, sizeof(a));
    listen(s, SOMAXCONN);

    while (true) {
        SOCKET c = accept(s, nullptr, nullptr);
        CreateThread(nullptr, 0, ClientThread, (LPVOID)c, 0, nullptr);
    }
}

void tcpClient() {
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);

    connect(s, (sockaddr*)&a, sizeof(a));

    Data d{1, "Hello JSON"};
    string msg = serialize(d);
    send(s, msg.c_str(), msg.size(), 0);

    char buf[1024];
    int n = recv(s, buf, sizeof(buf), 0);
    Data r = deserialize(string(buf, n));
    cout << r.id << " " << r.text << endl;

    closesocket(s);
    WSACleanup();
}

void udpServer() {
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(55000);
    a.sin_addr.s_addr = INADDR_ANY;
    bind(s, (sockaddr*)&a, sizeof(a));

    char buf[1024];
    sockaddr_in c{};
    int clen = sizeof(c);

    while (true) {
        int n = recvfrom(s, buf, sizeof(buf), 0, (sockaddr*)&c, &clen);
        sendto(s, buf, n, 0, (sockaddr*)&c, clen);
    }
}

void udpClient() {
    WSADATA w;
    WSAStartup(MAKEWORD(2,2), &w);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port = htons(55000);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);

    string msg;
    getline(cin, msg);

    sendto(s, msg.c_str(), msg.size(), 0, (sockaddr*)&a, sizeof(a));

    char buf[1024];
    int len = sizeof(a);
    recvfrom(s, buf, sizeof(buf), 0, (sockaddr*)&a, &len);
    cout << buf << endl;

    closesocket(s);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    string mode = argv[1];

    if (mode == "tcp_server") tcpServer();
    else if (mode == "tcp_client") tcpClient();
    else if (mode == "udp_server") udpServer();
    else if (mode == "udp_client") udpClient();

    return 0;
}

