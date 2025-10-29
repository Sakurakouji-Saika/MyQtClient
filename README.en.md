# QT_MyClient

This is a client application developed using the Qt framework, primarily implementing instant messaging features such as user login, friend management, message sending and receiving, file transfer, and profile settings.

## Features

- **User Login and Registration**: Users can log in to the server or register a new account.
- **Friend Management**: Supports adding and deleting friends, viewing the friend list and their statuses.
- **Message Sending and Receiving**: Supports real-time message exchange with chat history storage.
- **File Transfer**: Provides upload and download functionality, including avatar replacement.
- **Profile Settings**: Allows users to set personal information such as avatar and nickname.
- **Beautiful Interface**: Features a modern UI design with support for custom styling.

## Technology Stack

- **C++**: Primary programming language.
- **Qt**: Used for building the graphical user interface and network communication.
- **SQLite**: Local database for storing user data and chat records.
- **CMake**: Build system.

## Directory Structure

- `src/`: Source code directory containing all C++ source files.
- `src/DataBaseManage/`: Database management module handling interactions with the SQLite database.
- `src/Network/`: Network communication module handling message and file transfer between client and server.
- `src/app/`: Implementation of the main application window and login window.
- `src/utils/`: Utility classes and general-purpose functions.
- `src/widgets/`: Custom widgets.
- `ui/components/`: UI components such as chat windows and friend lists.
- `resources/`: Resource files including style sheets, images, and SVG icons.

## Build and Run

1. Clone the repository locally:
   ```bash
   git clone https://gitee.com/Sirin/QT_MyClient.git
   ```

2. Open and build the project using Qt Creator:
   - Launch Qt Creator and select "File" -> "Open File or Project".
   - Navigate to the cloned repository directory and select `CMakeLists.txt`.
   - Build and run the project.

## Usage Instructions

1. After launching the application, enter the server address and port to connect.
2. Log in to an existing account or register a new one.
3. Use the friend management feature to add or remove friends.
4. Communicate with friends in real time and send/receive files.
5. Update your profile and avatar in the settings menu.

## Contributors

Contributions and issue reports are welcome. Please follow the project's contribution guidelines when submitting PRs or issues.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.