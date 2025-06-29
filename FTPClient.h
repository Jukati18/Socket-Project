#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <winsock2.h>
#include <string>
#include <iostream>

using namespace std;

class FTPClient {
public:
    FTPClient();
    ~FTPClient();

    // Session management
    bool connect(const string& host, int port = 21);  // Chỉ kết nối, không đăng nhập
    bool manualLogin(const string& username, const string& password);  // Đăng nhập thủ công
    void disconnect();
    void setTransferMode(bool binary);
    void togglePassiveMode();
    void showStatus() const;

    // Utility functions
    bool isConnected() const { return m_connected; }
    bool isLoggedIn() const { return m_loggedIn; }  // Thêm trạng thái đăng nhập
    bool isPassiveMode() const { return m_passiveMode; }

private:
    SOCKET m_controlSocket;
    bool m_connected;
    bool m_loggedIn;
    bool m_binaryMode;
    bool m_passiveMode;
    string m_currentDir;
    string m_host;

    string sendCommand(const string& command);
    string readResponse();
};

#endif
