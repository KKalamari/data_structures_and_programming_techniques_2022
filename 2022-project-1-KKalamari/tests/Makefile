# paths
LIB = ../lib
INCLUDE = ../include
MODULES = ../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -g -I$(INCLUDE)
LDFLAGS = -lm
#-Werror

# Αρχεία .o
OBJS = state_test.o $(MODULES)/state_alt.o $(LIB)/k08.a $(LIB)/libraylib.a $(MODULES)/set_utils.o
# Το εκτελέσιμο πρόγραμμα
EXEC = state_test

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk


all: $(EXEC) $(MODULES)

# $(MODULES):
# 	$(CC) -c $(MODULES)/state_alt.c $(CFLAGS)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a