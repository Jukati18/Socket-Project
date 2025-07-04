#include "FTPClient.h"
#include <ws2tcpip.h>
#include <sstream>
#include <fstream>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

FTPClient::FTPClient()
    : m_controlSocket(INVALID_SOCKET),
      m_connected(false),
      m_loggedIn(false),
      m_binaryMode(true),
      m_passiveMode(false),
      promptConfirm(true)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
    }
}

FTPClient::~FTPClient() {
    if (m_connected) {
        disconnect();
    }
    WSACleanup();
}

bool FTPClient::connect(const std::string& host, int port) {
    if (m_connected) {
        cout << "Already connected to " << m_host << "\n";
        return true;
    }

    m_controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_controlSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        return false;
    }

    addrinfo hints{};
    addrinfo* result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    string portStr = to_string(port);

    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        cerr << "getaddrinfo failed: " << gai_strerror(status) << "\n";
        closesocket(m_controlSocket);
        return false;
    }

    if (::connect(m_controlSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        cerr << "Connection failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(m_controlSocket);
        return false;
    }

    freeaddrinfo(result);

    string response = readResponse();
    if (response.find("220") == string::npos) {
        cerr << "Connection refused: " << response << "\n";
        closesocket(m_controlSocket);
        return false;
    }

    m_host = host;
    m_connected = true;
    m_loggedIn = false;
    cout << "Connected to " << host << " (Not logged in yet)\n";
    return true;
}

bool FTPClient::manualLogin(const std::string& username, const std::string& password) {
    if (!m_connected) {
        cerr << "Not connected to server\n";
        return false;
    }

    if (m_loggedIn) {
        cout << "Already logged in as " << username << "\n";
        return true;
    }

    string userCmd = "USER " + username + "\r\n";
    if (send(m_controlSocket, userCmd.c_str(), userCmd.size(), 0) == SOCKET_ERROR) {
        cerr << "Failed to send USER command\n";
        return false;
    }

    string response = readResponse();
    if (response.find("331") == string::npos) {
        cerr << "USER command failed: " << response << "\n";
        return false;
    }

    string passCmd = "PASS " + password + "\r\n";
    if (send(m_controlSocket, passCmd.c_str(), passCmd.size(), 0) == SOCKET_ERROR) {
        cerr << "Failed to send PASS command\n";
        return false;
    }

    response = readResponse();
    if (response.find("230") == string::npos) {
        cerr << "Login failed: " << response << "\n";
        return false;
    }

    m_loggedIn = true;
    cout << "Logged in as " << username << "\n";
    return true;
}

void FTPClient::disconnect() {
    if (!m_connected) return;
    sendCommand("QUIT");
    closesocket(m_controlSocket);
    m_connected = false;
    m_loggedIn = false;
    cout << "Disconnected from " << m_host << "\n";
}

void FTPClient::setTransferMode(bool binary) {
    if (!m_connected) {
        cout << "Not connected\n";
        return;
    }
    m_binaryMode = binary;
    string response = sendCommand(binary ? "TYPE I" : "TYPE A");
    if (response.find("200") != string::npos) {
        cout << "Transfer mode set to " << (binary ? "binary" : "ASCII") << "\n";
    } else {
        cerr << "Failed to set transfer mode: " << response << "\n";
    }
}

void FTPClient::togglePassiveMode() {
    m_passiveMode = !m_passiveMode;
    cout << "Passive mode " << (m_passiveMode ? "enabled" : "disabled") << "\n";
    if (m_passiveMode) {
        string response = sendCommand("PASV");
        cout << "Server response: " << response << "\n";
    }
}

void FTPClient::showStatus() const {
    cout << "\n=== FTP Client Status ===\n";
    cout << "Connected: " << (m_connected ? "Yes" : "No") << "\n";
    if (m_connected) {
        cout << "Server: " << m_host << "\n";
    }
    cout << "Transfer mode: " << (m_binaryMode ? "Binary" : "ASCII") << "\n";
    cout << "Passive mode: " << (m_passiveMode ? "Enabled" : "Disabled") << "\n";
    cout << "========================\n\n";
}

std::string FTPClient::sendCommand(const std::string& command) {
    if (!m_connected) return "Not connected";
    string fullCommand = command + "\r\n";
    send(m_controlSocket, fullCommand.c_str(), fullCommand.length(), 0);
    return readResponse();
}

std::string FTPClient::readResponse() {
    char buffer[4096];
    int bytesReceived = recv(m_controlSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        m_connected = false;
        return "Connection lost";
    }
    return string(buffer, bytesReceived);
}

// =============================
// File transfer & utility logic
// =============================

SOCKET FTPClient::setupDataSocket(sockaddr_in& dataAddr, std::string& portCommand) {
    SOCKET dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    dataAddr.sin_family = AF_INET;
    dataAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    dataAddr.sin_port = 0;
    bind(dataSocket, (sockaddr*)&dataAddr, sizeof(dataAddr));
    int addrLen = sizeof(dataAddr);
    getsockname(dataSocket, (sockaddr*)&dataAddr, &addrLen);
    int port = ntohs(dataAddr.sin_port);
    listen(dataSocket, 1);
    int p1 = port / 256;
    int p2 = port % 256;
    portCommand = "PORT 127,0,0,1," + to_string(p1) + "," + to_string(p2) + "\r\n";
    return dataSocket;
}

bool FTPClient::scanWithClamAV(const std::string& filename) {
    SOCKET clamSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in clamAddr;
    clamAddr.sin_family = AF_INET;
    clamAddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &clamAddr.sin_addr);
    ::connect(clamSocket, (sockaddr*)&clamAddr, sizeof(clamAddr));
    ifstream file(filename, ios::binary);
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        send(clamSocket, buffer, file.gcount(), 0);
    }
    file.close();
    shutdown(clamSocket, SD_SEND);
    char result[100] = {};
    recv(clamSocket, result, sizeof(result) - 1, 0);
    closesocket(clamSocket);
    cout << "[SCAN] Ket qua quet virus: " << result << endl;
    return string(result).find("OK") != string::npos;
}

void FTPClient::uploadFile(const std::string& NameFile) {
    if (!scanWithClamAV(NameFile)) {
        cout << "File bi nhiem virus. Khong the upload.\n";
        return;
    }
    sockaddr_in dataAddr;
    string portCommand;
    SOCKET dataSocket = setupDataSocket(dataAddr, portCommand);
    send(m_controlSocket, portCommand.c_str(), portCommand.length(), 0);
    char buffer[1024];
    int recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    string storCommand = "STOR " + NameFile + "\r\n";
    send(m_controlSocket, storCommand.c_str(), storCommand.length(), 0);
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    sockaddr_in serverDataAddr;
    int serverDataAddrSize = sizeof(serverDataAddr);
    SOCKET dataConn = accept(dataSocket, (sockaddr*)&serverDataAddr, &serverDataAddrSize);
    ifstream file(NameFile, ios::binary);
    char fileBuffer[1024];
    while (file.read(fileBuffer, sizeof(fileBuffer)) || file.gcount())
        send(dataConn, fileBuffer, file.gcount(), 0);
    file.close();
    shutdown(dataConn, SD_SEND);
    closesocket(dataConn);
    closesocket(dataSocket);
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
}

void FTPClient::uploadMultipleFiles() {
    cout << "Nhap cac ten file muon upload (cach nhau boi dau cach): ";
    string line;
    getline(cin, line);
    istringstream iss(line);
    string fname;
    while (iss >> fname) {
        if (promptConfirm) {
            cout << "Upload file \"" << fname << "\"? (y/n): ";
            string c;
            getline(cin, c);
            if (c != "y" && c != "Y") continue;
        }
        uploadFile(fname);
    }
}

void FTPClient::downloadFile(const std::string& filename) {
    std::filesystem::create_directory("downloads");
    sockaddr_in dataAddr;
    string portCommand;
    SOCKET dataSocket = setupDataSocket(dataAddr, portCommand);
    send(m_controlSocket, portCommand.c_str(), portCommand.length(), 0);
    char buffer[1024];
    int recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    string retrCmd = "RETR " + filename + "\r\n";
    send(m_controlSocket, retrCmd.c_str(), retrCmd.length(), 0);
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    sockaddr_in serverDataAddr;
    int len = sizeof(serverDataAddr);
    SOCKET dataConn = accept(dataSocket, (sockaddr*)&serverDataAddr, &len);
    ofstream outFile("downloads/" + filename, ios::binary);
    while ((recvLen = recv(dataConn, buffer, sizeof(buffer), 0)) > 0) {
        outFile.write(buffer, recvLen);
    }
    outFile.close();
    closesocket(dataConn);
    closesocket(dataSocket);
    cout << "Da tai file ve: downloads/" << filename << endl;
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer << endl;
}

void FTPClient::downloadMultipleFiles() {
    cout << "Nhap cac ten file muon tai ve (cach nhau boi dau cach): ";
    string line;
    getline(cin, line);
    istringstream iss(line);
    string fname;
    while (iss >> fname) {
        if (promptConfirm) {
            cout << "Tai file \"" << fname << "\"? (y/n): ";
            char c;
            cin >> c;
            cin.ignore();
            if (c != 'y' && c != 'Y') continue;
        }
        downloadFile(fname);
    }
}

void FTPClient::listFiles() {
    sockaddr_in dataAddr;
    string portCommand;
    SOCKET dataSocket = setupDataSocket(dataAddr, portCommand);
    send(m_controlSocket, portCommand.c_str(), portCommand.length(), 0);
    char buffer[1024];
    int recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    string listCmd = "LIST\r\n";
    send(m_controlSocket, listCmd.c_str(), listCmd.length(), 0);
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer;
    sockaddr_in serverDataAddr;
    int len = sizeof(serverDataAddr);
    SOCKET dataConn = accept(dataSocket, (sockaddr*)&serverDataAddr, &len);
    cout << "Danh sach file tren server:\n";
    while ((recvLen = recv(dataConn, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[recvLen] = '\0';
        cout << buffer;
    }
    closesocket(dataConn);
    closesocket(dataSocket);
    recvLen = recv(m_controlSocket, buffer, sizeof(buffer) - 1, 0);
    buffer[recvLen] = '\0';
    cout << "[Server]: " << buffer << endl;
}

void FTPClient::togglePrompt() {
    promptConfirm = !promptConfirm;
    cout << "Xac nhan mget/mput: " << (promptConfirm ? "BAT" : "TAT") << endl;
}
