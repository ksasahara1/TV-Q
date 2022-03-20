SRC_DIR		:=	src
OBJ_DIR		:=	obj
BIN_DIR		:=	bin

CXX		:=	clang++
ifeq ($(shell uname -s), Darwin)
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=	-D__STDC_CONSTANT_MACROS
LDLIBS		:=	`pkg-config --libs-only-l libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale`
else
INCLUDE_DIRS	:=	
LIBRARY_DIRS	:=	
DFLAGS		:=
LDLIBS		:=	`pkg-config --libs-only-l libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale`
endif
OPT		:=	-O0 -g
CXXFLAGS	:=	-Wall $(OPT) $(foreach dir,$(INCLUDE_DIRS), -I$(dir)) `pkg-config --cflags libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale` $(DFLAGS)

LDFLAGS		:=	$(foreach dir,$(LIBRARY_DIRS), -I$(dir)) `pkg-config --libs-only-L libavcodec libavdevice libavfilter libavformat libavutil libpostproc libswresample libswscale`

TARGET		:=	$(BIN_DIR)/video_decoder

VPATH		:=  $(SRC_DIR):$(OBJ_DIR)
.SUFFIXES:	.cpp .o

all: $(TARGET)

$(TARGET): $(OBJ_DIR)/toImg.o
	mkdir -p $(dir $@)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -o $@ -c $(CXXFLAGS) $<

.PHONY: clean
clean:
	-rm -rfv $(OBJ_DIR) $(BIN_DIR) $(shell find . -name *~)
