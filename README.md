# Secure FTP Client with Virus Scanning via ClamAVAgent

## Table of Contents
1. [System Overview](#system-overview)
2. [Prerequisites](#prerequisites)
3. [Installation Guide](#installation-guide)
4. [Configuration](#configuration)
5. [Usage Instructions](#usage-instructions)
6. [Sample Commands & Expected Output](#sample-commands--expected-output)
7. [Detailed ClamAV Setup](#detailed-clamav-setup)
8. [Compiling the Source Code](#compiling-the-source-code)
9. [Running the System](#running-the-system)
10. [Security Notice](#security-notice)
11. [Example FTP Client Session](#example-ftp-client-session)
12. [Troubleshooting](#troubleshooting)
13. [Contact](#contact)

## 1. System Overview

This project delivers a **secure FTP client** with integrated virus scanning. The system is composed of three main components:

- **Custom FTP Client:** A C++ application offering FTP-like commands with enhanced security.
- **ClamAVAgent:** A service that scans files for viruses before upload.
- **FileZilla Server:** The FTP server that stores files after they pass the virus scan.

## 2. Prerequisites

### Hardware Requirements
- Three separate machines, or three terminal windows on a single machine.

### Software Requirements
- C++17 compatible compiler (e.g., g++ 9.0+)
- ClamAV antivirus (v0.103+)
- FileZilla Server (v0.9.60+)

## 3. Installation Guide

1. **Install FileZilla Server**  
   Download and install from the official website (https://filezilla-project.org/download.php?type=server).

2. **Install ClamAV**  
   Download and install the latest version from the official ClamAV site (https://www.clamav.net/downloads).

## 4. Configuration

### 4.1 FileZilla Server

- Launch FileZilla Server Interface.
- Navigate to **Edit → Users**.
- Add a new user:
  - **Username:** SinhVien
  - **Password:** 123456789
  - **Home Directory:** `D:/Test`
- Set permissions as needed (read/write/delete, etc.).

### 4.2 ClamAV

- Ensure ClamAV is installed and the virus database is updated.
- Confirm the path to `clamscan.exe` and the database directory in your code matches your installation.

## 5. Usage Instructions

- Start the **ClamAVAgent** service (default port: 8888).
- Start **FileZilla Server** (default port: 21).
- Run the compiled FTP client executable.

## 6. Sample Commands & Expected Output

### Basic Navigation
| Command                  | Description                  |
|--------------------------|-----------------------------|
| `ls`                     | List files                  |
| `cd documents`           | Change directory            |
| `pwd`                    | Show current directory      |
| `mkdir secure_folder`    | Create directory            |

### File Operations
| Command                          | Description                        |
|-----------------------------------|------------------------------------|
| `put report.pdf`                  | Upload single file (scanned first) |
| `mput *.txt`                      | Upload multiple text files         |
| `get important.docx`              | Download file                      |
| `delete old_file.zip`             | Delete file                        |
| `rename draft.txt final.txt`      | Rename file                        |

### Session Management
| Command      | Description                    |
|--------------|-------------------------------|
| `binary`     | Set binary transfer mode       |
| `ascii`      | Set ASCII transfer mode        |
| `passive`    | Toggle passive mode            |
| `status`     | Show connection status         |
| `help`       | Display command help           |
| `quit`       | Exit the client                |

Connect to server:
FTP> connect ftp.example.com 21
if successful: Connected to ftp.example.com (Not logged in yet)
if failed: Connection failed [error code]

Login:
FTP> login
Username: [user enters username]
Password: [user enters password]
if successful: Logged in as [username]
if failed: Login failed: [server response]

Set transfer mode:
FTP> setmode binary
Transfer mode set to binary

Toggle passive mode:
FTP> passive
Passive mode enabled
Server response: 227 Entering Passive Mode (192,168,1,100,15,203)
PASV mode ready. Connect to 192.168.1.100:4043

FTP> status
=== FTP Client Status ===
Connected: Yes
Server: ftp.example.com
Logged in: Yes
Transfer mode: Binary
Passive mode: Enabled
========================

Disconnect:
FTP> disconnect
Disconnected from ftp.example.com

Help:
FTP> help
- connect <host> [port] - Connect to FTP server
- login                 - Login to server
- disconnect            - Disconnect from server
- setmode <binary|ascii>- Set transfer mode
- passive               - Toggle passive mode
- status                - Show connection status
- help                  - Show this help
- quit                  - Exit the program

FTP> quit
Disconnected from [host]
Goodbye!

## 7. Detailed ClamAV Setup

- Download and install ClamAV.
- Create a virus database folder, e.g., `F:\ClamAV\database`.
- Update the database:
  ```sh
  freshclam --datadir=F:\ClamAV\database
  ```
- Ensure the code references the correct paths for `clamscan.exe` and the database.

## 8. Compiling the Source Code

- Use a C++17 compatible compiler (e.g., g++ from MinGW or MSYS2).
- In the source directory, compile with:
  ```sh
  g++ -std=c++17 -o ftpclient main.cpp FTPClient.cpp FTPClient.h -lws2_32 -lstdc++fs
  g++ -std=c++17 -o clamav_agent main_clamav.cpp Clamav_agent.cpp Clamav_agent.h -lws2_32 -lstdc++fs
  ```
- If you encounter missing library errors, verify your compiler installation and library paths.

## 9. Running the System

1. Start the **ClamAVAgent** service (port 8888).
2. Start **FileZilla Server** (port 21).
3. Run the FTP client:
   ```sh
   ./ftpclient
   ```

## 10. Security Notice

- **Authentication is required** before using upload/download commands (`put`, `get`, `mput`, `mget`).
- Only files that pass the ClamAV scan are uploaded.
- Adjust all file paths in the code to match your system if your directory structure differs.

## 11. Example FTP Client Session

```sh
ftp> connect 127.0.0.1 21
ftp> login SinhVien 123456789
ftp> put file1.txt
ftp> get file2.txt
ftp> ls
ftp> quit
```

## 12. Troubleshooting

- Ensure you are logged in before uploading or downloading files.
- Verify **ClamAVAgent** is running and accessible on port 8888.
- Confirm **FileZilla Server** is running and the user has correct permissions.
- For Linux users, adjust Windows-style paths and compilation commands as needed.

## 13. Contact

For further support, please contact the developer or refer to this README for guidance.
