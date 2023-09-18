COMPILER=g++ -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

BUILD_DIR=build
SOURCES_DIR=src
SOURCES=$(BUILD_DIR)/$(SOURCES_DIR)/stack.cpp



.PHONY: all makeDirs


all: makeDirs main.exe

makeDirs:
ifeq (,$(wildcard ./$(BUILD_DIR)))
	mkdir $(BUILD_DIR)
endif
ifeq (,$(wildcard ./$(BUILD_DIR)/$(SOURCES_DIR)))
	mkdir ./$(BUILD_DIR)/$(SOURCES_DIR)
endif


main.exe: $(BUILD_DIR)/main.o
	@echo Compiling main.exe
	@echo __________________
	@echo __________________
	@$(COMPILER) $^ -o $@
	@echo Compiled main.o


$(BUILD_DIR)/main.o: main.cpp main.h
	@echo Compiling main.o
	@echo ________________
	@echo ________________
	@$(COMPILER) -c $< -o $@
	@echo Compiled main.o
