CC=clang
CXX=clang++
CFLAGS=-std=c11 -Weverything -Werror -Wno-unused-macros -Wno-reserved-id-macro -Wno-padded
CXXFLAGS=-std=c++17 -Weverything -Werror -Wno-unused-macros -Wno-reserved-id-macro -Wno-padded -Wno-c++98-compat

z: lex.yy.o z.tab.o ast.o
	$(CXX) -o z $^

z.tab.h: z.y
	bison --debug --verbose -d z.y

z.tab.c: z.y
	bison -d z.y

lex.yy.c: z.l z.tab.h
	flex z.l

