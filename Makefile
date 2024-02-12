CC = gcc
CFLAGS = -g -Wall
AR = ar -cvq
RANLIB = ranlib
PROJ_DIR = .
SRC_DIR = ./src
INC_DIR = $(PROJ_DIR)/include
LIB_DIR = $(PROJ_DIR)/lib
TEST_DIR = $(PROJ_DIR)/test
ULTS_SRC = $(SRC_DIR)/ult_round_robin.c $(SRC_DIR)/ult_mutex.c $(SRC_DIR)/queue.c
ULTS_OBJ = $(patsubst %.c,%.o,$(ULTS_SRC))
HEADER = $(SRC_DIR)/ult.h $(SRC_DIR)/queue.h
LIBRARY = libult.a

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(ULTS_OBJ) library

library: $(ULTS_OBJ)
	$(AR) $(LIB_DIR)/$(LIBRARY) $(ULTS_OBJ)
	$(RANLIB) $(LIB_DIR)/$(LIBRARY)
	cp -f $(HEADER) $(INC_DIR)

copy:
	cp -f $(HEADER) $(INC_DIR)

clean:
	$(RM) -f *.o $(LIB_DIR)/* $(ULT_OBJ)
