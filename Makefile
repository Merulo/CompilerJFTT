CXX = g++
LD = g++

COMMON_FLAGS = -Wall
COMMON_FLAGS += -pedantic
COMMON_FLAGS += -std=c++17
COMMON_FLAGS += -I$(SOURCE_DIR)

CXX_FLAGS = $(COMMON_FLAGS)
CXX_FLAGS += -c
CXX_FLAGS += -O1
CXX_FLAGS += -MMD

LD_FLAGS = $(COMMON_FLAGS)
LD_FLAGS += -lpthread

SOURCE_DIR 				= src
BINARY_DIR 				= bin
TEST_DIR   				= test
EXTERNAL_DIR			= src/External
REGISTER_MACHINE_DIR 	= registerMachine/registerMachine

MAIN = $(SOURCE_DIR)/Main.cpp

ALL_SOURCES = $(shell find $(SOURCE_DIR) -type f -name *.cpp ! -iname Parser.cpp ! -iname Scanner.cpp)
ALL_TESTS = $(shell find $(TEST_DIR) -type d)
SOURCES = $(filter-out $(MAIN), $(ALL_SOURCES))

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%,$(BINARY_DIR)/%,$(OBJECTS))

all: start main.exe
.PHONY : main.exe

start:
	flex -o $(EXTERNAL_DIR)/Scanner.cpp $(EXTERNAL_DIR)/scanner.l
	bison -d -o $(EXTERNAL_DIR)/Parser.cpp $(EXTERNAL_DIR)/parser.y

main.exe: $(MAIN) $(OBJECTS)
	$(LD) $(LD_FLAGS) $(OBJECTS) $(MAIN) -o $@
	(cd $(REGISTER_MACHINE_DIR) && make maszyna-rejestrowa)
	(mv $(REGISTER_MACHINE_DIR)/maszyna-rejestrowa emulator.exe)
	echo $(ALL_TESTS) | xargs -n 1 cp main.exe
	echo $(ALL_TESTS) | xargs -n 1 cp emulator.exe
	echo $(ALL_TESTS) | xargs -n 1 cp emulator.py

$(BINARY_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(dir $@)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $< -o $@

clear:
	rm -rf $(BINARY_DIR)/
	rm *.exe


-include $(OBJECTS:%.o=%.d)
