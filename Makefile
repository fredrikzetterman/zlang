LLVM_BIN_PATH := /usr/bin

LLVM_CXXFLAGS := `$(LLVM_BIN_PATH)/llvm-config --cxxflags`
LLVM_LDFLAGS := `$(LLVM_BIN_PATH)/llvm-config --ldflags --libs --system-libs`

CC:=clang
CXX:=clang++
CFLAGS:=-std=c11 -Weverything -Werror -Wno-unused-macros -Wno-reserved-id-macro -Wno-padded -Wno-format-nonliteral -include zlang.h -g -O0
CXXFLAGS:=-std=c++17 -Weverything -Werror -Wno-unused-macros -Wno-reserved-id-macro -Wno-padded -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unknown-warning-option -include zlang.h -g -O0 $(LLVM_CXXFLAGS)
LDFLAGS:=$(LLVM_LDFLAGS)

z: z.lex.o z.tab.o ast.o ir.o
	$(CXX) -o z $^ $(LDFLAGS)

z.tab.h: z.y
	bison --debug --verbose -d z.y

z.tab.c: z.y
	bison -d z.y

z.lex.c: z.l z.tab.h
	flex z.l
clean:
	@rm -f *.o z z.tab.* z.lex.* z.output z.stackdump

