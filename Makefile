COMPILER=g++ -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Wno-pointer-arith -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

BUILD_DIR=build
SOURCES_FILE_NAME=src
SOURCES_DIR=$(BUILD_DIR)/$(SOURCES_FILE_NAME)
STACK_FILES=$(SOURCES_DIR)/stack.cpp $(SOURCES_DIR)/stack.h
HASH_FILES=$(SOURCES_DIR)/hash.cpp $(SOURCES_DIR)/hash.h
O_FILES=$(BUILD_DIR)/main.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/hash.o
SOURCES=$(STACK_FILES)



.PHONY: all makeDirs


all: makeDirs main.exe

makeDirs:
ifeq (,$(wildcard ./$(BUILD_DIR)))
	mkdir $(BUILD_DIR)
endif
ifeq (,$(wildcard ./$(SOURCES_DIR)))
	mkdir $(SOURCES_DIR)
endif


main.exe: $(O_FILES)
	@echo Compiling main.exe
	@echo __________________
	@echo __________________
	@$(COMPILER) $^ -o $@


$(BUILD_DIR)/main.o: main.cpp main.h
	@echo Compiling main.o
	@echo ________________
	@echo ________________
	@$(COMPILER) -c $< -o $@
	@echo Compiled main.o


$(BUILD_DIR)/stack.o: $(STACK_FILES)
	@echo Compiling stack.o
	@echo ________________
	@echo ________________
	@$(COMPILER) -c $< -o $@
	@echo Compiled stack.o


$(BUILD_DIR)/hash.o: $(HASH_FILES)
	@echo Compiling hash.o
	@echo ________________
	@echo ________________
	@$(COMPILER) -c $< -o $@
	@echo Compiled hash.o
