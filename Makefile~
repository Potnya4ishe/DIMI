CC      = g++ 
CFLAGS  = -c -std=c++11 -g -lpthread -lm -pedantic -Wall -Winvalid-pch
LDFLAGS = 
TARGET  = server

all: $(TARGET) client

$(TARGET): server.o Token.o Lexer.o Logger.o ASTtree.o Parser.o Interpreter.o Exception.o TokenHolder.o
	$(CC) server.o Token.o Lexer.o Logger.o ASTtree.o Parser.o Interpreter.o Exception.o TokenHolder.o -lpthread -o $(TARGET)

client: client.o Token.o Lexer.o Logger.o ASTtree.o Parser.o Interpreter.o Exception.o TokenHolder.o
	$(CC) client.o Token.o Lexer.o Logger.o ASTtree.o Parser.o Interpreter.o Exception.o TokenHolder.o -lpthread -o client

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

Exception.o: Exception.cpp
	$(CC) $(CFLAGS) Exception.cpp

Logger.o: Logger.cpp
	$(CC) $(CFLAGS) Logger.cpp

Token.o: Token.cpp
	$(CC) $(CFLAGS) Token.cpp

server.o: server.cpp
	$(CC) $(CFLAGS) server.cpp

Lexer.o: Lexer.cpp 
	$(CC) $(CFLAGS) Lexer.cpp

ASTtree.o: ASTtree.cpp
	$(CC) $(CFLAGS) ASTtree.cpp

Parser.o: Parser.cpp
	$(CC) $(CFLAGS) Parser.cpp

Interpreter.o: Interpreter.cpp
	$(CC) $(CFLAGS) Interpreter.cpp

TokenHolder.o: TokenHolder.cpp
	$(CC) $(CFLAGS) TokenHolder.cpp


clean:
	rm -rf *.o
