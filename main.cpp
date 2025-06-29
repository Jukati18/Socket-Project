#include "FTPClient.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void showHelp() {
    cout << "\nFTP Client Commands:\n"
        << "  connect <host> [port] - Connect to FTP server\n"
        << "  login <user> <pass>   - Login to FTP server\n"
        << "  disconnect            - Disconnect from server\n"
        << "  binary                - Set binary transfer mode\n"
        << "  ascii                 - Set ASCII transfer mode\n"
        << "  passive               - Toggle passive mode\n"
        << "  status                - Show current status\n"
        << "  help,?                - Show this help\n"
        << "  quit,exit             - Exit the program\n\n";
}

void handleConnect(FTPClient& client, istringstream& iss) {
    string host;
    int port = 21;

    if (!(iss >> host)) {
        cout << "Usage: connect <host> [port]\n";
        return;
    }

    // Kiểm tra xem có port không
    if (iss >> port) {
        // Nếu có port
        if (port < 1 || port > 65535) {
            cout << "Invalid port number. Using default port 21.\n";
            port = 21;
        }
    }

    if (client.connect(host, port)) {
        cout << "Connected to " << host << ":" << port << "\n";
        cout << "Use 'login <username> <password>' to authenticate.\n";
    }
    else {
        cout << "Failed to connect to " << host << ":" << port << "\n";
    }
}

void handleLogin(FTPClient& client, istringstream& iss) {
    string username, password;

    if (!(iss >> username >> password)) {
        cout << "Usage: login <username> <password>\n";
        return;
    }

    if (!client.isConnected()) {
        cout << "Not connected to any server. Use 'connect' first.\n";
        return;
    }

    if (client.manualLogin(username, password)) {
        cout << "Login successful!\n";
    }
    else {
        cout << "Login failed. Please check your credentials.\n";
    }
}

int main() {
    FTPClient client;
    string input;

    cout << "Simple FTP Client with Manual Login\n";
    showHelp();

    while (true) {
        cout << "ftp> ";
        getline(cin, input);

        if (input.empty()) continue;

        istringstream iss(input);
        string command;
        iss >> command;

        if (command == "connect") {
            handleConnect(client, iss);
        }
        else if (command == "login") {
            handleLogin(client, iss);
        }
        else if (command == "disconnect") {
            client.disconnect();
            cout << "Disconnected from server.\n";
        }
        else if (command == "binary") {
            client.setTransferMode(true);
            cout << "Binary transfer mode set.\n";
        }
        else if (command == "ascii") {
            client.setTransferMode(false);
            cout << "ASCII transfer mode set.\n";
        }
        else if (command == "passive") {
            client.togglePassiveMode();
            cout << "Passive mode " << (client.isPassiveMode() ? "enabled" : "disabled") << ".\n";
        }
        else if (command == "status") {
            client.showStatus();
        }
        else if (command == "help" || command == "?") {
            showHelp();
        }
        else if (command == "quit" || command == "exit") {
            client.disconnect();
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }

    return 0;
}