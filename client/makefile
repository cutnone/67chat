# Claude taught me how to automatically
# compile the whole project.
# there is no way I am going to manually
# create entries in this thing for
# more than 5 files lol.

ifeq ($(OS), Windows_NT)
    # Windows settings
    LIBS = -l:pdcurses.a
    INCLUDES = -I C:/ProgramData/mingw64/mingw64/include
    LIBDIRS = -L C:/ProgramData/mingw64/mingw64/lib
    EXT = .exe
else
	LIBS = -lncurses
	EXT = .out
endif

CC = gcc
FLAGS = -o
DIRS = . ui ui/scenes ui/components ../common/datastructures
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c)) 
# generates a list of all c files in the specified source directories

OBJ = $(SRC:.c=.o) # source list with .c replaced with .o

all: $(OBJ)
	gcc $(FLAGS) client$(EXT) $(OBJ) $(INCLUDES) $(LIBDIRS) $(LIBS)

%.o: %.c
	gcc -c -o $@ $^ $(INCLUDES)

clean:
ifeq ($(OS), Windows_NT)
	del /s /q $(subst /,\,$(OBJ))
else
	rm -f $(OBJ)
endif

debug:
	gcc -g $(FLAGS) client$(EXT) $(OBJ) $(INCLUDES) $(LIBDIRS) $(LIBS)