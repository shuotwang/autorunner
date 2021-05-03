AR=ar
AS=as
CC=gcc
CPP=cpp
CXX=g++
LD=ld
OBJCOPY = objcopy
OBJDUMP = objdump
STRIP = strip

INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

#DEBUG_MODE=TRUE

PKGS = #gtk+-3.0 

ifdef DEBUG_MODE
DEFINES  += -DDEBUG
CFLAGS   += -g -ggdb -D_GLIBCXX_DEBUG
#LDFLAGS  += -Wl,-no_pie
else 
CFLAGS   += -O3
endif

INCLUDE  += -I $(INC_DIR)
CFLAGS   +=  -Wall `pkg-config --cflags $()`
LDFLAGS  +=`pkg-config --libs $()` 
#LDFLAGS += -lgdk_imlib
CPPFLAGS += -std=c++14 

APP_NAME = autograder

APP_OBJS = $(OBJ_DIR)/AutoRunner.o                    \
    $(OBJ_DIR)/CommandsResponder.o            \


all: directories $(BIN_DIR)/$(APP_NAME)

$(BIN_DIR)/$(APP_NAME): $(APP_OBJS)
	$(CXX) $(APP_OBJS) -o $(BIN_DIR)/$(APP_NAME) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(LDFLAGS) 
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@
	
directories: $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)