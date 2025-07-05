Dưới đây là phiên bản đã được **định dạng lại rõ ràng và đẹp mắt** cho file `README.md` của bạn theo chuẩn Markdown:

````markdown
# Secure FTP Client with Virus Scanning via ClamAVAgent

---

## 📖 Table of Contents

1. [System Overview](#system-overview)  
2. [Prerequisites](#prerequisites)  
3. [Installation Guide](#installation-guide)  
4. [Configuration](#configuration)  
5. [Usage Instructions](#usage-instructions)  
6. [Sample Commands and Expected Output](#sample-commands-and-expected-output)  
7. [Detailed ClamAV Setup](#7-detailed-clamav-setup)  
8. [Compiling the Source Code](#8-compiling-the-source-code)  
9. [Running the System](#9-running-the-system)  
10. [Security Notice](#10-security-notice)  
11. [Example FTP Client Session](#11-example-ftp-client-session)  
12. [Troubleshooting](#12-troubleshooting)  
13. [Contact](#13-contact)  

---

## 1. System Overview

This project implements a secure FTP client with integrated virus scanning functionality.  
The system consists of **three main components**:

1. **Custom FTP Client**  
   A C++ application that provides FTP-like commands with additional security features.

2. **ClamAVAgent**  
   A scanning service that checks files for viruses before they're uploaded to the FTP server.

3. **FileZilla Server**  
   The standard FTP server that stores files after they've been cleared by the virus scanner.

---

## 2. Prerequisites

### 🔧 Hardware Requirements

- Three machines (or three terminal windows on one machine)

### 💻 Software Requirements

- C++17 compatible compiler (e.g. `g++ 9.0` or higher)  
- ClamAV antivirus (`v0.103+`)  
- FileZilla Server (`v0.9.60+`)

---

## 3. Installation Guide

- **Install FileZilla Server**  
  👉 [https://filezilla-project.org/download.php?type=server](https://filezilla-project.org/download.php?type=server)

- **Install ClamAV**  
  Follow instructions on the official website.

---

## 4. Configuration

### 1️⃣ FileZilla Server Configuration

1. Launch **FileZilla Server Interface**  
2. Go to `Edit → Users`  
3. Add a new user:  
   - Username: `SinhVien`  
   - Password: `123456789`  
   - Home directory: `D:/Test`  
4. Set appropriate permissions (read/write/delete/etc.)

### 2️⃣ ClamAV Configuration

Refer to section [7. Detailed ClamAV Setup](#7-detailed-clamav-setup)

---

## 5. Usage Instructions

Run the FTP client in terminal after starting the ClamAVAgent and FileZilla Server.

---

## 6. Sample Commands and Expected Output

### 📁 Basic Navigation

```bash
ls                    # List files
cd documents          # Change directory
pwd                   # Show current directory
mkdir secure_folder   # Create directory
````

### 📤📥 File Operations

```bash
put report.pdf                  # Upload single file (scanned first)
mput *.txt                      # Upload multiple text files
get important.docx              # Download file
delete old_file.zip             # Delete file
rename draft.txt final.txt      # Rename file
```

### 🔒 Session Management

```bash
binary          # Set binary transfer mode
ascii           # Set ASCII transfer mode
passive         # Toggle passive mode
status          # Show connection status
help            # Display command help
quit            # Exit the client
```

---

## 7. Detailed ClamAV Setup

1. Download ClamAV and install.
2. Create a virus database folder, e.g., `F:\ClamAV\database`.
3. Update database:

```sh
freshclam --datadir=F:\ClamAV\database
```

4. Ensure correct paths to `clamscan.exe` and database in your code, e.g., `F:\ClamAV\clamscan.exe`.

---

## 8. Compiling the Source Code

Ensure you have a C++17 compatible compiler (e.g., MinGW or MSYS2).

Compile:

```sh
g++ -std=c++17 -o ftpclient main.cpp FTPClient.cpp Clamav_agent.cpp -lws2_32 -lstdc++fs
```

> If errors occur, verify libraries and compiler configuration.

---

## 9. Running the System

1. Start **ClamAVAgent** (default port `8888`).
2. Start **FileZilla Server** (default port `21`).
3. Run FTP Client:

```sh
./ftpclient
```

---

## 10. Security Notice

* You **must login** before using `put`, `get`, `mput`, `mget`.
* Only virus-free files (scanned via ClamAV) are uploaded.
* Adjust file paths in code to match your local machine setup.

---

## 11. Example FTP Client Session

```sh
ftp> connect 127.0.0.1 21
ftp> login SinhVien 123456789
ftp> put file1.txt
ftp> get file2.txt
ftp> ls
ftp> quit
```

---

## 12. Troubleshooting

* Cannot upload/download? → Check login status.
* Cannot scan file? → Ensure ClamAVAgent is running on port `8888`.
* FileZilla not working? → Ensure correct user permissions are set.
* On Linux? → Adjust all Windows paths and compilation commands accordingly.

---

## 13. Contact

For further support, please contact the developer or refer to this README for guidance.

