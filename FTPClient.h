#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#pragma once
#include <string>
#include <winsock2.h>
#include <iostream>
#include <algorithm>

class FTPClient {
public:
    FTPClient();
    ~FTPClient();

    // Session management
    bool connect(const std::string& host, int port = 21);  // Kết nối server
    bool manualLogin(const std::string& username, const std::string& password);  // Đăng nhập
    void disconnect();
    void setTransferMode(bool binary);
    void togglePassiveMode();
    void showStatus() const;

    // File transfer & utility
    void uploadFile(const std::string& filename);
    void uploadMultipleFiles();
    void downloadFile(const std::string& filename);
    void downloadMultipleFiles();
    void listFiles();
    void togglePrompt();
    bool changeDirectory(const std::string& path);
    bool printWorkingDirectory();
    bool makeDirectory(const std::string& dirname);
    bool removeDirectory(const std::string& dirname);
    bool deleteFile(const std::string& filename);
    bool renameFile(const std::string& from, const std::string& to);

    // State
    bool isConnected() const { return m_connected; }
    bool isLoggedIn() const { return m_loggedIn; }
    bool isPassiveMode() const { return m_passiveMode; }

    //Recursive upload/dowload folders
    void uploadFolderRecursive(const std::string& localPath, const std::string& remotePath);
    void downloadFolderRecursive(const std::string& remotePath, const std::string& localPath);

private:
    // Socket & session
    SOCKET m_controlSocket;
    bool m_connected;
    bool m_loggedIn;
    bool m_binaryMode;
    bool m_passiveMode;
    std::string m_pasvIp;
    int m_pasvPort;
    std::string m_currentDir;
    std::string m_host;

    // Data connection & scan
    bool promptConfirm;
    SOCKET setupDataSocket(sockaddr_in& dataAddr, std::string& portCommand);
    bool scanWithClamAV(const std::string& filename);

    // FTP protocol
    std::string sendCommand(const std::string& command);
    std::string readResponse();
};

#endif
