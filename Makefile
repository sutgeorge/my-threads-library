CC = gcc
CFLAGS = -g -Wall
AR = ar -cvq
RANLIB = ranlib
PROJ_DIR = .
SRC_DIR = ./src
INC_DIR = $(PROJ_DIR)/include
LIB_DIR = $(PROJ_DIR)/lib
TEST_DIR = $(PROJ_DIR)/tests
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

1_basic_test: $(ULTS_OBJ)
	$(CC) -o $(TEST_DIR)/1_basic_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/1_basic_test.c
	./$(TEST_DIR)/1_basic_test

2_thread_joins_and_values_returned: $(ULTS_OBJ)
	$(CC) -o $(TEST_DIR)/2_thread_joins_and_values_returned -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/2_thread_joins_and_values_returned.c
	./$(TEST_DIR)/2_thread_joins_and_values_returned

3_expected_proper_exit: $(ULTS_OBJ)
	$(CC) -o $(TEST_DIR)/3_expected_proper_exit -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/3_expected_proper_exit.c
	./$(TEST_DIR)/3_expected_proper_exit

4_spawning_threads_recursively: $(ULTS_OBJ)
	$(CC) -o $(TEST_DIR)/4_spawning_threads_recursively -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/4_spawning_threads_recursively.c
	./$(TEST_DIR)/4_spawning_threads_recursively

run_tests: 1_basic_test 2_thread_joins_and_values_returned 3_expected_proper_exit 4_spawning_threads_recursively

copy:
	cp -f $(HEADER) $(INC_DIR)

clean:
	$(RM) -f *.o $(LIB_DIR)/* $(ULT_OBJ)
