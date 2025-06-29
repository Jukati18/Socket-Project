#include "FTPClient.h"
#include <ws2tcpip.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

FTPClient::FTPClient() :
    m_controlSocket(INVALID_SOCKET),
    m_connected(false),
    m_loggedIn(false),
    m_binaryMode(true),
    m_passiveMode(false)
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

bool FTPClient::connect(const string& host, int port) {
    if (m_connected) {
        cout << "Already connected to " << m_host << "\n";
        return true;
    }

    m_controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_controlSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        return false;
    }

    // Modern address resolution using getaddrinfo
    addrinfo hints{};
    addrinfo* result = nullptr;

    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_STREAM; // Stream socket
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    // Convert port number to string
    string portStr = to_string(port);

    // Resolve the server address and port
    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        cerr << "getaddrinfo failed: " << gai_strerror(status) << "\n";
        closesocket(m_controlSocket);
        return false;
    }

    // Attempt to connect to the first address returned
    if (::connect(m_controlSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        cerr << "Connection failed: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(m_controlSocket);
        return false;
    }

    // Free the address information
    freeaddrinfo(result);

    // Read welcome message
    string response = readResponse();
    if (response.find("220") == string::npos) {
        cerr << "Connection refused: " << response << "\n";
        closesocket(m_controlSocket);
        return false;
    }

    //Update State
    m_host = host;
    m_connected = true;
    m_loggedIn = false; // Chưa đăng nhập
    cout << "Connected to " << host << " (Not logged in yet)\n";

    return true;
}

bool FTPClient::manualLogin(const string& username, const string& password) {
    if (!m_connected) {
        cerr << "Not connected to server\n";
        return false;
    }

    if (m_loggedIn) {
        cout << "Already logged in as " << username << "\n";
        return true;
    }

    // Send USER command
    string userCmd = "USER " + username + "\r\n";
    if (send(m_controlSocket, userCmd.c_str(), userCmd.size(), 0) == SOCKET_ERROR) {
        cerr << "Failed to send USER command\n";
        return false;
    }

    // Read server response (expect 331 for password request)
    string response = readResponse();
    if (response.find("331") == string::npos) {
        cerr << "USER command failed: " << response << "\n";
        return false;
    }

    // Send PASS command
    string passCmd = "PASS " + password + "\r\n";
    if (send(m_controlSocket, passCmd.c_str(), passCmd.size(), 0) == SOCKET_ERROR) {
        cerr << "Failed to send PASS command\n";
        return false;
    }

    // Read server response (expect 230 for successful login)
    response = readResponse();
    if (response.find("230") == string::npos) {
        cerr << "Login failed: " << response << "\n";
        return false;
    }


    //Update State
    m_loggedIn = true;
    cout << "Logged in as " << username << "\n";
    return true;
}

void FTPClient::disconnect() {
    if (!m_connected) return;

    sendCommand("QUIT");
    closesocket(m_controlSocket);
    m_connected = false;
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
    }
    else {
        cerr << "Failed to set transfer mode: " << response << "\n";
    }
}

void FTPClient::togglePassiveMode() {
    m_passiveMode = !m_passiveMode;
    cout << "Passive mode " << (m_passiveMode ? "enabled" : "disabled") << "\n";
    if (m_passiveMode) {
        // Actually send PASV command to server
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

// Helper functions
string FTPClient::sendCommand(const string& command) {
    if (!m_connected) return "Not connected";

    string fullCommand = command + "\r\n";
    send(m_controlSocket, fullCommand.c_str(), fullCommand.length(), 0);
    return readResponse();
}

string FTPClient::readResponse() {
    char buffer[4096];
    int bytesReceived = recv(m_controlSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        m_connected = false;
        return "Connection lost";
    }
    return string(buffer, bytesReceived);
}
