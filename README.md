# 🎉 poll-tcp-db-server-client - Simple TCP Client-Server Application

## 📥 Download Now
[![Download Latest Release](https://img.shields.io/github/v/release/taher-03/poll-tcp-db-server-client?label=Download&logo=github)](https://github.com/taher-03/poll-tcp-db-server-client/releases)

## 📖 Introduction
Welcome to the **poll-tcp-db-server-client** application. This program is a TCP client-server setup written in C, allowing you to manage employee information with basic functions like adding, removing, updating, and listing records. 

This application utilizes poll IO multiplexing, making it efficient for handling multiple client connections. It's designed for users who need a simple way to manage employee data over a network.

## 🚀 Getting Started
Before you begin, ensure that you have the following prerequisites:

### ✅ System Requirements
- **Operating System:** Windows, macOS, or Linux
- **C Compiler:** GCC or Clang
- **Network Connection:** Required for client-server operation
- **Basic Command Line Knowledge:** You will need to run commands in your terminal or command prompt

## 💡 Features
- **Add Employee**: Quickly input new employee records.
- **Remove Employee**: Easily delete records from the database.
- **Update Employee**: Modify existing employee details in seconds.
- **List Employees**: View all employee records at a glance.
- **Multiplexing Support**: Handle multiple client requests efficiently.

## 📥 Download & Install
To download the application, please visit this page: [GitHub Releases](https://github.com/taher-03/poll-tcp-db-server-client/releases).

1. Click on the link above to go to the Releases page.
2. Locate the latest release.
3. Download the appropriate file for your operating system.
4. Follow the installation steps below.

## ⚙️ Installation Steps
### For Windows:
1. Open the Command Prompt.
2. Navigate to the folder where you downloaded the application.
   - Use the command: `cd path\to\your\download\folder`
3. Run the application with the following command:
   - `poll-tcp-db-server-client.exe`

### For macOS/Linux:
1. Open the Terminal.
2. Navigate to the folder where you downloaded the application.
   - Use the command: `cd path/to/your/download/folder`
3. Make the application executable (only needed for Linux):
   - `chmod +x poll-tcp-db-server-client`
4. Run the application with the command:
   - `./poll-tcp-db-server-client`

## 🛠️ Usage Instructions
After starting the application, you will see a command prompt interface. Here are some commands you can use:

- **Add Employee**: Type `add [EmployeeID] [Name] [Position]` to add an employee.
- **Remove Employee**: Type `remove [EmployeeID]` to delete an employee.
- **Update Employee**: Type `update [EmployeeID] [NewName] [NewPosition]` to change details.
- **List Employees**: Type `list` to see all current employees.

For example, to add an employee, you would type:
```
add 1 John Doe Developer
```

## 🌐 Networking Setup
This application requires a network connection to work as a server and to connect clients. Ensure that your firewall settings allow network connections for this program. 

You might also want to specify the port and IP address the server should listen on. The default is usually `localhost` and `8080`.

## 🔧 Troubleshooting
If you encounter issues, here are some common solutions:

- **Cannot connect to server**: Ensure that the server is running. Check your firewall settings to allow connections through the program.
- **Application crashes**: Make sure you are using the correct commands. Refer to the usage instructions above.

## 📝 Contribution
If you want to contribute to this project, feel free to submit an issue or pull request. Your suggestions and improvements are welcome!

## 📄 License
This project is licensed under the MIT License. 

You can find more details on the license by checking the LICENSE file in the repository. 

## 🔗 Links
- [GitHub Repository](https://github.com/taher-03/poll-tcp-db-server-client)
- [Download Latest Release](https://github.com/taher-03/poll-tcp-db-server-client/releases) 

Thank you for using poll-tcp-db-server-client! We hope this application helps you manage your employee data effectively.