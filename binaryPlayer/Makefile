CXX	:=	arm-linux-gnueabi-g++
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
