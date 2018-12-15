CXX = g++
LD = g++

COMMON_FLAGS = -Wall
COMMON_FLAGS += -pedantic
COMMON_FLAGS += -std=c++17
COMMON_FLAGS += -I$(SOURCE_DIR)

CXX_FLAGS = $(COMMON_FLAGS)
CXX_FLAGS += -c
CXX_FLAGS += -O3
CXX_FLAGS += -MMD

LD_FLAGS = $(COMMON_FLAGS)
LD_FLAGS += -lpthread

SOURCE_DIR = src
BINARY_DIR = bin

MAIN = $(SOURCE_DIR)/Main.cpp

ALL_SOURCES = $(shell find $(SOURCE_DIR) -type f -name *.cpp ! -iname Parser.cpp ! -iname Scanner.cpp)
SOURCES = $(filter-out $(MAIN), $(ALL_SOURCES))

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%,$(BINARY_DIR)/%,$(OBJECTS))

all: start main.exe
.PHONY : main.exe

start:
	flex -o $(SOURCE_DIR)/Scanner.cpp $(SOURCE_DIR)/scanner.l
	bison -d -o $(SOURCE_DIR)/Parser.cpp $(SOURCE_DIR)/parser.y

main.exe: $(MAIN) $(OBJECTS)
	$(LD) $(LD_FLAGS) $(OBJECTS) $(MAIN) -o $@
	cp main.exe test/main.exe

$(BINARY_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(dir $@)
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $< -o $@

clear:
	rm -rf $(BINARY_DIR)/
	rm *.exe


-include $(OBJECTS:%.o=%.d)
