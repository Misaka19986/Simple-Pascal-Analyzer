# Makefile for kuma

GCC = gcc
G++ = g++

TARGET = ./runme

TEST1 = ../test/test1.pas
TEST2 = ../test/test2.pas

# Compile
compile: kuma.cpp lexer.cpp parser.cpp
	$(G++) -o $(TARGET) $^ -std=c++17

test1:
	$(TARGET) $(TEST1) -p

test2:
	$(TARGET) $(TEST2) -p

# Clean generated file
clean:
	$(RM) middle.* $(TARGET)