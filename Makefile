# Makefile for Flex and Bison

LEX = flex
YACC = bison
GCC = gcc
G++ = g++

# Define the target executable
TARGET = runme

# Compile the target
$(TARGET): $(SOURCES)
	$(CC) -o $@ $(LDFLAGS) $^

# Compile the Bison-generated C file
parse: parser.y
	$(YACC) --defines=parser.hpp -o $@ $^

# Compile the Flex-generated C file
scan: lexer.l
	$(LEX) --header-file=lexer.h $^

# Compile all
scan_and_parse: lexer.l parser.y
	$(LEX) lexer.l
	$(YACC) --defines=parser.hpp -o $@ $^

scan_and_test: lexer.l
	$(RM) *.o scan.c parser.hpp parser.c lexer.hpp target.c lex.yy.* out.* $(TARGET)
	$(LEX) --header-file=lexer.h $^
	$(GCC) lex.yy.c -o $(TARGET) -lfl
	./$(TARGET) sample.pas

compile_c: 
	$(GCC) lex.yy.c -o $(TARGET) -lfl

compile_c++:
	$(G++) lex.yy.cc -o $(TARGET) -lfl

# Clean up generated files
clean:
	$(RM) *.o scan.c parser.hpp parser.c lexer.hpp target.c lex.yy.* out.* $(TARGET)
