# Makefile for kuma

GCC = gcc
G++ = g++

TARGET = runme

TEST1 = ../test/test1.pas

# Compile
compile: kuma.cpp lexer.cpp
	$(G++) -o $(TARGET) $^ -std=c++17

test1:
	./$(TARGET) $(TEST1)

# Clean generated file
clean:
	$(RM) out.* $(TARGET)