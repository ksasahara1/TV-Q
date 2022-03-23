ifeq ($(shell uname -s),Linux)
ifeq ($(shell uname -m),armv6l)
PREFIX := 
else 
PREFIX		:=	arm-linux-gnueabi-
endif
else
PREFIX		:=	arm-linux-gnueabi-
endif
CXX		:=	$(PREFIX)g++
CXX_FLAGS	:=	-O2 -c
LDFLAGS		:=
LDLIBS		:=

VPATH		:= ./src:./obj

bin/playBinary: obj/playBinary.o
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)
obj/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $(CXX_FLAGS) $< 
.PHONY: clean
clean:
	@rm -rfv bin/ obj/ $(shell find . -name *~)
