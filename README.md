# connect4

Simple implementation of the classic Connect 4 game in C, using a client-server architecture with sockets. The game consists of a server and two clients, allowing two players to compete.

# Installation & Compilation

Ensure you have gcc and make installed, then compile the program by running:

```
make
```

This will generate the necessary binaries in the bin/ directory.

# Running the Game

## Start the Server:

```
./bin/server
```

The server will wait for two clients to connect.

## Start the Clients:
Each client represents a player in the game. On two separate terminals, run:

```
./bin/client
```

# Cleanup

To remove compiled binaries, run:

```
make clean
```
