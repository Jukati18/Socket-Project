#include "Clamav_agent.h"
#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
atomic<bool> scanning_done(false);

ClamAVAgent::ClamAVAgent() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

ClamAVAgent::~ClamAVAgent() {
    WSACleanup();
}

void ClamAVAgent::showProgressBar_realTime() {
    const int barWidth = 30;
    int progress = 0;
    const int estimatedScanTimeSec = 15;
    const int updateDelayMs = estimatedScanTimeSec * 1000 / barWidth;
    while (!scanning_done && progress <= barWidth) {
        int percent = (progress * 100) / barWidth;
        cout << "\r[Agent] Quet virus: [";
        for (int i = 0; i < barWidth; ++i) {
            cout << (i < progress ? "#" : "-");
        }
        cout << "] " << percent << "%" << flush;
        this_thread::sleep_for(chrono::milliseconds(updateDelayMs));
        progress++;
    }
    if (scanning_done) {
        cout << "\r[Agent] Quet virus: [##############################] 100%\n";
    }
}

std::string ClamAVAgent::scanWithClamAV(const std::string& filePath) {
    string cmd = "F:\\ClamAV\\clamscan.exe --database=F:\\ClamAV\\database \"" + filePath + "\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        cerr << "[Agent] Loi: Khong the mo process clamscan.\n";
        return "ERROR";
    }
    char buffer[512];
    bool infected = false;
    scanning_done = false;
    thread progressThread(&ClamAVAgent::showProgressBar_realTime, this);
    while (fgets(buffer, sizeof(buffer), pipe)) {
        string line(buffer);
        if (line.find("FOUND") != string::npos)
            infected = true;
        cout << "\n" << line;
    }
    _pclose(pipe);
    scanning_done = true;
    progressThread.join();
    return infected ? "INFECTED" : "OK";
}

void ClamAVAgent::run() {
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    cout << "[Agent] ClamAVAgent dang lang nghe 8888...\n";
    while (true) {
        SOCKET client = accept(serverSocket, NULL, NULL);
        cout << "[Agent] Nhan ket noi tu client.\n";
        ofstream outFile("temp_upload.txt", ios::binary);
        if (!outFile) {
            cerr << "[Agent] Khong the mo file de tam ghi.\n";
            closesocket(client);
            continue;
        }
        char buffer[1024];
        int bytes;
        cout << "[Agent] Dang nhan du lieu...\n";
        while ((bytes = recv(client, buffer, sizeof(buffer), 0)) > 0) {
            outFile.write(buffer, bytes);
        }
        outFile.close();
        cout << "[Agent] Nhan xong file, bat dau quet virus...\n";
        string result = scanWithClamAV("temp_upload.txt");
        cout << "[Agent] Ket qua quet: " << result << endl;
        send(client, result.c_str(), result.length(), 0);
        closesocket(client);
    }
    closesocket(serverSocket);
}

