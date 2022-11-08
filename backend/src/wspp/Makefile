TARGET := $(shell basename $(shell pwd))
TOP_DIR ?= $(abspath $(shell pwd)/../../..)
OS_NAME ?= $(shell uname -s)
CPU_TYPE ?= $(shell uname -m)
BUILD_TYPE ?= Release
INSTALL_PREFIX := $(TOP_DIR)/backend/bin/${OS_NAME}/$(CPU_TYPE)/${BUILD_TYPE}

############################# Choose the compiler #############################
CROSS_COMPILE :=
CROSS?=
GCC := $(CROSS)gcc
GXX := $(CROSS)g++
AR :=  $(CROSS)ar
#############################  Define Micro Variables #############################

############################# Add Source files #############################
SOURCEPATH := $(CURDIR)
SOURCES := $(wildcard $(SOURCEPATH)/*.cpp)

############################# Add Include directories #############################
INCLUDEPATH := -I$(TOP_DIR)/backend/3rd/websocketpp -I$(TOP_DIR)/backend/3rd/asio/asio/include -I../common
INCLUDEPATH += -I$(TOP_DIR)/backend/inc

############################# Add libs #############################
LIBS := -L$(TOP_DIR)/backend/lib/
LIBS += -lstdc++ -lgcc_s
LIBS += -lpthread -lrt -ldl

############################# Add CFLAGS, CXXFLAGS and LDFLAGS  #############################
CFLAGS := -fPIC -Wall -W -Wextra -Wno-unused-parameter -pthread -Wl,-Bsymbolic -D__LINUX__ -DLINUX #-fvisibility=hidden 
CXXFLAGS := -fPIC -Wall -W -Wextra -Wno-unused-parameter -std=gnu++11 -pthread -Wl,-Bsymbolic -D__LINUX__ -DLINUX #-fvisibility=hidden 
LDFLAGS := -fPIC -Wl,-Bsymbolic #-fvisibility=hidden 

############################# Build Type: Debug or Release #############################
ifeq ($(BUILD_TYPE), Debug)
    # debug
    CFLAGS += -g -DDEBUG
	CXXFLAGS += -g -DDEBUG
else
    # release
    CFLAGS += -O2 -DNDEBUG
	CXXFLAGS += -O2 -DNDEBUG
endif

#############################  Config library name and path to store  #############################
LIBRARY_INSTALL_PATH := $(INSTALL_PREFIX)/lib
INCLUDE_INSTALL_PATH := $(INSTALL_PREFIX)/inc

#######################################################################################
#############################  Begin the Make procedures  #############################
BUILD_PATH := $(TOP_DIR)/backend/build/$(OS_NAME)/$(CPU_TYPE)/${BUILD_TYPE}/$(TARGET)
$(shell mkdir -p $(BUILD_PATH))

OBJECTS := $(patsubst %.cpp, $(BUILD_PATH)/%.o, $(notdir $(SOURCES)))

EXE_FILE := $(INSTALL_PREFIX)/$(TARGET)
$(shell mkdir -p $(INSTALL_PREFIX))

.PHONY: all
all: $(EXE_FILE)

############################# Depend #############################
$(BUILD_PATH)/main.o: $(TOP_DIR)/backend/inc/web_assets_basews.h

############################# Making #############################
$(EXE_FILE): $(OBJECTS) $(TOP_DIR)/backend/lib/${OS_NAME}/$(CPU_TYPE)/${BUILD_TYPE}/libcommon.a
	@echo .......  Linking  ........
	$(GXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_PATH)/%.o : $(SOURCEPATH)/%.cpp
	$(GXX) -c $(CXXFLAGS) $(INCLUDEPATH) -o $@ $<

.PHONY: clean

clean:
	rm -rf $(BUILD_PATH) $(EXE_FILE)
