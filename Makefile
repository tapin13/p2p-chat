CC=gcc
CFLAGS=
TARGET=p2p-chat

p2pchat: p2p-chat.c
	@$(CC) $(TARGET).c -o $(TARGET)
	
clean:
	@rm -rf $(TARGET)