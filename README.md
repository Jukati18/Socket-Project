Secure FTP Client with Virus Scanning via ClamAVAgent

Table of Contents
1.System Overview
2.Prerequisites
3.Installation Guide
4.Configuration
5.Usage Instructions
6.Sample Commands
7.Troubleshooting
8.Security Notes
9.License

System Overview
This project implements a secure FTP client with integrated virus scanning functionality. The system consists of three main components:
1.Custom FTP Client: A C++ application that provides FTP-like commands with additional security features
2.ClamAVAgent: A scanning service that checks files for viruses before they're uploaded to the FTP server
3.FileZilla Server: The standard FTP server that stores files after they've been cleared by the virus scanner
