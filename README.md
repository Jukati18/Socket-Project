Secure FTP Client with Virus Scanning via ClamAVAgent

Table of Contents
1.System Overview
2.Prerequisites
3.Installation Guide
4.Configuration
5.Usage Instructions
6.Sample Commands and Expected Output

System Overview
This project implements a secure FTP client with integrated virus scanning functionality. The system consists of three main components:
1.Custom FTP Client: A C++ application that provides FTP-like commands with additional security features
2.ClamAVAgent: A scanning service that checks files for viruses before they're uploaded to the FTP server
3.FileZilla Server: The standard FTP server that stores files after they've been cleared by the virus scanner

Prerequisites
Hardware Requirements
Three machines (or three terminal windows on one machine)
Software Requirements
C++17 compatible compiler (g++ 9.0 or higher)
ClamAV antivirus (version 0.103 or higher)
FileZilla Server (version 0.9.60 or higher)

Installation Guide
Install FileZilla Server: https://filezilla-project.org/download.php?type=server
Install ClamAV

Configuration
1.FileZilla Server Configuration
Launch FileZilla Server Interface
Go to Edit → Users
Add a new user with:
Username: SinhVien
Password: 123456789
Home directory: D:/Test
Set appropriate permissions (read/write/delete/etc.)
2.ClamAV Configuration

Usage Instructions

Sample Commands and Expected Output
Basic Navigation
ls                  # List files
cd documents        # Change directory
pwd                 # Show current directory
mkdir secure_folder # Create directory
File Operations
put report.pdf                  # Upload single file (scanned first)
mput *.txt                      # Upload multiple text files
get important.docx              # Download file
delete old_file.zip             # Delete file
rename draft.txt final.txt      # Rename file
Session Management
binary          # Set binary transfer mode
ascii           # Set ASCII transfer mode
passive         # Toggle passive mode
status          # Show connection status
help            # Display command help
quit            # Exit the client

## Additional Notes and Detailed Setup

### 7. Detailed ClamAV Setup

- Download ClamAV from the official website and install it.
- Create a folder for the virus database, for example: `F:\ClamAV\database`.
- Update the virus database using:
  ```sh
  freshclam --datadir=F:\ClamAV\database
  ```
- Ensure the path to `clamscan.exe` and the database in your code matches your installation, e.g., `F:\ClamAV\clamscan.exe`.

### 8. Compiling the Source Code

- Make sure you have a C++17 compatible compiler (e.g., g++ from MinGW or MSYS2).
- Open a terminal in the source code directory.
- Compile the project with:
  ```sh
  g++ -std=c++17 -o ftpclient main.cpp FTPClient.cpp Clamav_agent.cpp -lws2_32 -lstdc++fs
  ```
- If you encounter missing library errors, check your compiler installation and ensure all required libraries are present.

### 9. Running the System

- Start the ClamAVAgent service (it listens on port 8888 by default).
- Start FileZilla Server and ensure it is running and listening on port 21.
- Run the compiled FTP client:
  ```sh
  ./ftpclient
  ```

### 10. Security Notice

- You must log in before using file upload/download commands (`put`, `get`, `mput`, `mget`). The client enforces authentication for these operations.
- Only files that pass the ClamAV scan will be uploaded to the server.
- Adjust all file paths in the code to match your system if you use a different directory structure.

### 11. Example FTP Client Session

```sh
ftp> connect 127.0.0.1 21
ftp> login SinhVien 123456789
ftp> put file1.txt
ftp> get file2.txt
ftp> ls
ftp> quit
```

### 12. Troubleshooting

- If you cannot upload or download files, check that you are logged in.
- Verify that ClamAVAgent is running and accessible on port 8888.
- Ensure FileZilla Server is running and the user has correct permissions.
- For Linux users, adjust all Windows-style paths and compilation commands accordingly.

### 13. Contact

For further support, please contact the developer or refer to this README for guidance.

