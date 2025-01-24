# Variables
CC = gcc
CFLAGS = -Wall -Iinclude
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Targets
SERVER_TARGET = $(BINDIR)/server
CLIENT_TARGET = $(BINDIR)/client

# Source files
SERVER_SRC = $(SRCDIR)/game_state.c $(SRCDIR)/server_socket.c $(SRCDIR)/server_main.c
CLIENT_SRC = $(SRCDIR)/game_state.c $(SRCDIR)/client_socket.c $(SRCDIR)/client_main.c

# Object files
SERVER_OBJ = $(SERVER_SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Rules
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Build server
$(SERVER_TARGET): $(SERVER_OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Build client
$(CLIENT_TARGET): $(CLIENT_OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Rule to build object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
