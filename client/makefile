# Claude taught me how to automatically
# compile the whole project.
# there is no way I am going to manually
# create entries in this thing for
# more than 5 files lol.

CC = gcc

DIRS = . ui ui/scenes ui/components ../common/datastructures
SRC = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c)) 
# generates a list of all c files in the specified source directories

OBJ = $(SRC:.c=.o) # source list with .c replaced with .o

all: $(OBJ)
	gcc -o client.out $(OBJ) -lncurses

%.o: %.c
	gcc -c -o $@ $^

clean:
	rm -f **/*.o