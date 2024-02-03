.PHONY: header all clean file_tree run

CC_FLAGS = -Wall -Wextra -pedantic -g
LD_FLAGS = -Wall -pedantic -g -lm

CC = gcc
OBJ = $(shell find src -name '*.c' | sed -e 's/\.c/\.o/' -e 's/src/obj/')

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CC_FLAGS)

main: file_tree $(OBJ)
	$(CC) $(OBJ) -o bin/main $(LD_FLAGS)

sdl: CC_FLAGS += -DMAKE_SDL
sdl: LD_FLAGS += -lSDL2
sdl: main

tice: CC_FLAGS += -DMAKE_TICE
tice: main

ppm: CC_FLAGS += -DMAKE_PPM
ppm: main

header: HEADER_NAME = $(shell basename $(FILE) | tr a-z A-Z | sed 's/\./_/')
header: SRC_FILE = $(shell echo $(FILE) | sed 's/\.h/\.c/')
header: 
ifdef FILE
ifeq ("$(wildcard $(FILE))", "")
	@touch $(FILE) 
	@touch $(SRC_FILE)
	@echo '#ifndef $(HEADER_NAME)' >> $(FILE)
	@echo '#define $(HEADER_NAME)' >> $(FILE)
	@echo '' >> $(FILE)
	@echo '' >> $(FILE)
	@echo '' >> $(FILE)
	@echo '#endif // $(HEADER_NAME)' >> $(FILE)
	@echo '#include "$(shell basename $(FILE))"' >> $(SRC_FILE)

	@echo "Created c header file ($(FILE)) with header guard ($(HEADER_NAME)) and source file ($(SRC_FILE))."
	# git add $(FILE) $(SRC_FILE)
else
	@echo "Header file already exists."
endif
else
	@echo "Please specify a header file name."
endif

run:
	@./bin/main

view_ppm: ppm
	@mkdir -p out
	@./bin/main > out/result.ppm
	@gthumb out/result.ppm

all: clean file_tree main

file_tree:
	mkdir -p bin $(shell find src -type d | sed 's/src/obj/g')

clean:
	rm -rf bin obj

