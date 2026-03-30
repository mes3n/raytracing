-include .config

.PHONY: all clean reset

CC_FLAGS=-Wall -Wextra -pedantic -g -Ofast -Iexternal
LD_FLAGS=-Wall -Wextra -pedantic -g -Ofast -lm -lpthread

CC=gcc
TARGET=bin/main
OBJ=$(shell find src -name '*.c' | sed -e 's/\.c/\.o/' -e 's/src/obj/')

CONFIG_FRONTEND ?= sdl
CONFIG_STB_IMAGE ?= y

ifeq ($(CONFIG_FRONTEND),sdl)
	CC_FLAGS += -DMAKE_SDL
	LD_FLAGS += -lSDL2
endif

ifeq ($(CONFIG_STB_IMAGE),n)
	CC_FLAGS += -DNO_STB_IMAGE
endif

$(TARGET): $(OBJ)
	@mkdir -p $(shell dirname $@)
	$(CC) $^ -o $@ $(LD_FLAGS)

obj/%.o:: src/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CC_FLAGS)

ifneq ($(CONFIG_STB_IMAGE),n)
src/render/rtw_image.c: external/stb_image.o
$(TARGET): external/stb_image.o
endif

external/stb_image.o: CC_FLAGS += -DSTB_IMAGE_IMPLEMENTATION
external/stb_image.o: external/stb_image.h
	@echo "[ INFO ] Precompiling $@"
	$(CC) -x c -c $< -o $@ $(CC_FLAGS)

external/stb_image.h:
	@mkdir -p $(shell dirname $@)
	@echo "[ INFO ] Downloading dependency $@"
	curl -o $@ https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h

all: clean $(TARGET)

clean:
	rm -rf bin obj

reset: clean
	rm -rf external

