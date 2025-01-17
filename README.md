# ft_irc

**ft_irc** is a lightweight IRC (Internet Relay Chat) server built from scratch in C. The project adheres to the basic functionalities outlined in the RFC 2812 standard, enabling users to connect via an IRC client, join channels, and communicate in real-time.

## Features

- **Client Connections**: Handles multiple client connections using sockets.
- **Authentication**: Supports basic user authentication with nicknames and usernames.
- **Channels**: Allows users to create, join, and leave channels.
- **Messaging**:
  - Private messages between users.
  - Public messages within channels.
- **Operator Privileges**: Grants operators the ability to manage channels (e.g., kick users, set topics).
- **Error Handling**: Provides informative error messages for invalid commands or misuse.
- **Compliance**: Implements essential features from the IRC protocol.

## Getting Started

### Prerequisites

- A Unix-like operating system (Linux or macOS).
- GCC or a compatible C compiler.
- An IRC client (e.g., HexChat, irssi, or WeeChat) for testing.

### Installation

1. Clone the repository:
   ```bash
   git clone git@github.com:d2iv3r/ft_irc.git
   cd ft_irc
   ```
2. Compile the project:
   ```bash
   make
   ```
3. Run the server:
   ```bash
   ./ft_irc <port> <password>
   ```
   Example:
   ```bash
   ./ft_irc 6667 mypassword
   ```

## Usage

1. Start the server using the steps above.
2. Connect to the server using an IRC client:
   - Open your IRC client.
   - Connect to `localhost` on the specified port (e.g., `6667`).
   - Provide the server password if required.
3. Use IRC commands to interact with the server:
- `PASS <password>:` used to set a 'connection password'.
- `NICK <nickname>:` used to give user a nickname or change    the previous one.
- `USER <username> <hostname> <servername> <realname>:` The USER message is used at the beginning of connection to specify the username, hostname, servername and realname of s new user.
- `JOIN <channel>{,<channel>} [<key>{,<key>}]:` Join a channel (the correct key (password) must be given if it is set.).
- `INVITE <nickname> <channel>:` invite users to a channel.
- `KICK <channel> <user> [<comment>]:` remove a user from a channel
- `PRIVMSG <receiver>{,<receiver>} <text to be sent>:` send private messages between users. (<receiver> can also be a list of names or channels separated with commas.)
- `TOPIC <channel> [<topic>]:` change or view the topic of a channel.
- `MODE  <channel> {[+|-]|i|t|k|o|l} [<key>|<limit>|<user>]:` change the characteristics of a channel:
   - `i:` - Set/remove Invite-only channel;
   - `t:` - Set/remove the restrictions of the TOPIC command to   channel operators;
   - `k:` - Set/remove the channel key (password);
   - `o:` - Give/take channel operator privilege;
   - `l:` - Set/remove the user limit to channel;
- `QUIT [<Quit message>]:` Disconnect from the server.

## Bonus: Bot Feature, KICKI

As a bonus, the project includes a **moderator bot** that monitors messages in a channel and automatically kicks users who use inappropriate or offensive language.

### How It Works

1. The bot listens to all public messages in a channel.
2. If a message contains a word from a predefined list of "bad words," the bot issues a warning.
3. On repeated offense or severe violations, the bot kicks the user from the channel.

### Configuration

- The list of "bad words" can be customized in the bot's configuration file or source code.
- The bot's threshold for kicking users can also be adjusted (e.g., after one or multiple offenses).


### Example Usage

1. Start the IRC server.
2. Connect to the server and invite the bot to a channel and set it as operator:
   ```bash
   INVITE KICKI #channel
   MODE #channel +o KICKI
   ```

3. The bot will automatically monitor messages and moderate the channel.

## Teammates

This project was developed collaboratively by:

- **Hamza Chahboune**:  [GitHub Profile](https://github.com/hachahbo)
