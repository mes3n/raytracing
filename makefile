-include .config

.PHONY: all clean reset

CC_FLAGS=-Wall -Wextra -pedantic -g -Ofast -Iexternal -I.
LD_FLAGS=-Wall -Wextra -pedantic -g -Ofast -lm -lpthread

CC=gcc
TARGET=bin/main
OBJ=$(shell find src -name '*.c' | sed -e 's/\.c/\.o/' -e 's/src/obj/')

CONFIG_FRONTEND ?= sdl
CONFIG_STB_IMAGE ?= y
CONFIG_OPENCL_BACKEND ?= y

ifeq ($(CONFIG_FRONTEND),sdl)
	CC_FLAGS += -DSDL_FRONTEND
	LD_FLAGS += -lSDL2
endif

ifeq ($(CONFIG_STB_IMAGE),n)
	CC_FLAGS += -DNO_STB_IMAGE
endif

ifneq ($(CONFIG_OPENCL_BACKEND),n)
	LD_FLAGS += -lOpenCL
endif

$(TARGET): $(OBJ)
	@mkdir -p $(shell dirname $@)
	$(CC) $^ -o $@ $(LD_FLAGS)

obj/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CC_FLAGS)

ifneq ($(CONFIG_STB_IMAGE),n)
src/render/rtw_image.c: external/stb_image.o
$(TARGET): external/stb_image.o
endif

ifneq ($(CONFIG_OPENCL_BACKEND),n)
src/render/cl_loader.c: external/CL
endif

external/stb_image.h:
	@mkdir -p $(shell dirname $@)
	@echo "[ INFO ] Downloading dependency $@"
	curl -o $@ https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h

external/stb_image.o: CC_FLAGS += -DSTB_IMAGE_IMPLEMENTATION
external/stb_image.o: external/stb_image.h
	@echo "[ INFO ] Precompiling $@"
	$(CC) -x c -c $< -o $@ $(CC_FLAGS)


external/CL: DL_PATH = external/OpenCL-Headers
external/CL:
	@mkdir -p $(shell dirname $@)
	@echo "[ INFO ] Downloading dependency $@"
	git clone https://github.com/KhronosGroup/OpenCL-Headers $(DL_PATH)
	mv -v $(DL_PATH)/CL $@
	rm -rf $(DL_PATH)


all: clean $(TARGET)

clean:
	rm -rf bin obj

reset: clean
	rm -rf external

