#include "FTPClient.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void showHelp()
{
    cout << "\nFTP Client Commands:\n"
         << " connect <host> [port] - Ket noi toi FTP server\n"
         << " login <user> <pass>   - Dang nhap\n"
         << " disconnect            - Ngat ket noi\n"
         << " binary                - Chuyen sang che do binary\n"
         << " ascii                 - Chuyen sang che do ASCII\n"
         << " passive               - Bat/tat passive mode\n"
         << " status                - Xem trang thai\n"
         << " put                   - Upload 1 file (co scan)\n"
         << " mput                  - Upload nhieu file\n"
         << " get                   - Tai 1 file ve\n"
         << " mget                  - Tai nhieu file ve\n"
         << " ls                    - Xem danh sach file tren server\n"
         << " prompt                - Bat/tat xac nhan khi mget/mput\n"
         << " help, ?               - Xem huong dan\n"
         << " quit, exit            - Thoat chuong trinh\n\n";
}

int main()
{
    FTPClient client;
    string input;

    cout << "Simple FTP Client\n";
    showHelp();

    while (true)
    {
        cout << "ftp> ";
        getline(cin, input);
        if (input.empty())
            continue;
        istringstream iss(input);
        string command;
        iss >> command;

        if (command == "connect")
        {
            string host;
            int port = 21;
            iss >> host;
            if (iss >> port)
            {
                if (port < 1 || port > 65535)
                    port = 21;
            }
            if (host.empty())
            {
                cout << "Usage: connect <host> [port]\n";
                continue;
            }
            if (client.connect(host, port))
            {
                cout << "Connected to " << host << ":" << port << "\n";
                cout << "Use 'login <username> <password>' to authenticate.\n";
            }
            else
                cout << "Failed to connect.\n";
        }
        else if (command == "login")
        {
            string user, pass;
            iss >> user >> pass;
            if (user.empty() || pass.empty())
            {
                cout << "Usage: login <user> <pass>\n";
                continue;
            }
            if (client.manualLogin(user, pass))
                cout << "Login successful!\n";
            else
                cout << "Login failed.\n";
        }
        else if (command == "disconnect")
        {
            client.disconnect();
            cout << "Disconnected.\n";
        }
        else if (command == "binary")
        {
            client.setTransferMode(true);
        }
        else if (command == "ascii")
        {
            client.setTransferMode(false);
        }
        else if (command == "passive")
        {
            client.togglePassiveMode();
        }
        else if (command == "status")
        {
            client.showStatus();
        }
        else if (command == "put")
        {
            if (!client.isLoggedIn())
            {
                cout << "Ban phai dang nhap truoc khi upload file.\n";
                continue;
            }
            cout << "Nhap ten file de upload: ";
            string fname;
            getline(cin, fname);
            client.uploadFile(fname);
        }
        else if (command == "mput")
        {
            client.uploadMultipleFiles();
        }
        else if (command == "get")
        {
            cout << "Nhap ten file de tai ve: ";
            string fname;
            getline(cin, fname);
            client.downloadFile(fname);
        }
        else if (command == "mget")
        {
            client.downloadMultipleFiles();
        }
        else if (command == "ls")
        {
            client.listFiles();
        }
        else if (command == "prompt")
        {
            client.togglePrompt();
        }
        else if (command == "help" || command == "?")
        {
            showHelp();
        }
        else if (command == "quit" || command == "exit")
        {
            client.disconnect();
            cout << "Goodbye!\n";
            break;
        }
        else
        {
            cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
    return 0;
}
