CC = gcc
CFLAGS = -g -Wall
AR = ar -cvq
RANLIB = ranlib
PROJ_DIR = .
SRC_DIR = ./src
INC_DIR = $(PROJ_DIR)/include
LIB_DIR = $(PROJ_DIR)/lib
TEST_DIR = $(PROJ_DIR)/tests
ULTS_SRC = $(SRC_DIR)/ult_round_robin.c $(SRC_DIR)/ult_mutex.c $(SRC_DIR)/queue.c $(SRC_DIR)/list.c $(SRC_DIR)/graph.c
ULTS_OBJ = $(patsubst %.c,%.o,$(ULTS_SRC))
HEADER = $(SRC_DIR)/ult.h $(SRC_DIR)/queue.h $(SRC_DIR)/list.h $(SRC_DIR)/graph.h
LIBRARY = libult.a

### The '@' symbol inserted before each command in the test targets are supposed to suppress GCC echo output
### in the terminal (this is just to notice which test cases passed/failed)

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

all: $(ULTS_OBJ) library

library: $(ULTS_OBJ)
	@$(AR) $(LIB_DIR)/$(LIBRARY) $(ULTS_OBJ)
	@$(RANLIB) $(LIB_DIR)/$(LIBRARY)
	@cp -f $(HEADER) $(INC_DIR)

1_basic_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/1_basic_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/1_basic_test.c
	@./$(TEST_DIR)/1_basic_test

2_thread_joins_and_values_returned: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/2_thread_joins_and_values_returned -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/2_thread_joins_and_values_returned.c
	@./$(TEST_DIR)/2_thread_joins_and_values_returned

3_expected_proper_exit: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/3_expected_proper_exit -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/3_expected_proper_exit.c
	@./$(TEST_DIR)/3_expected_proper_exit

4_spawning_threads_recursively: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/4_spawning_threads_recursively -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/4_spawning_threads_recursively.c
	@./$(TEST_DIR)/4_spawning_threads_recursively

5_verifying_thread_id_equality_check: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/5_verifying_thread_id_equality_check -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/5_verifying_thread_id_equality_check.c
	@./$(TEST_DIR)/5_verifying_thread_id_equality_check

6_verifying_thread_self: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/6_verifying_thread_self -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/6_verifying_thread_self.c
	@./$(TEST_DIR)/6_verifying_thread_self

7_expected_proper_thread_cancellation: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/7_expected_proper_thread_cancellation -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/7_expected_proper_thread_cancellation.c
	@./$(TEST_DIR)/7_expected_proper_thread_cancellation

8_mutex_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/8_mutex_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/8_mutex_test.c
	@./$(TEST_DIR)/8_mutex_test

9_thread_yield_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/9_thread_yield_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/9_thread_yield_test.c
	@./$(TEST_DIR)/9_thread_yield_test

10_producer_consumer_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/10_producer_consumer_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/10_producer_consumer_test.c
	@./$(TEST_DIR)/10_producer_consumer_test

11_threads_joining_each_other: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/11_threads_joining_each_other -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/11_threads_joining_each_other.c
	@./$(TEST_DIR)/11_threads_joining_each_other

list_1_basic_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/list_1_basic_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/list_1_basic_test.c
	@./$(TEST_DIR)/list_1_basic_test

list_2_popping_elements: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/list_2_popping_elements -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/list_2_popping_elements.c
	@./$(TEST_DIR)/list_2_popping_elements

list_3_removing_elements: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/list_3_removing_elements -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/list_3_removing_elements.c
	@./$(TEST_DIR)/list_3_removing_elements

list_4_find_an_element: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/list_4_find_an_element -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/list_4_find_an_element.c
	@./$(TEST_DIR)/list_4_find_an_element

graph_1_basic_test: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_1_basic_test -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_1_basic_test.c
	@./$(TEST_DIR)/graph_1_basic_test

graph_2_count_nodes: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_2_count_nodes -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_2_count_nodes.c
	@./$(TEST_DIR)/graph_2_count_nodes

graph_3_count_edges: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_3_count_edges -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_3_count_edges.c
	@./$(TEST_DIR)/graph_3_count_edges

graph_4_detecting_cycles: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_4_detecting_cycles -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_4_detecting_cycles.c
	@./$(TEST_DIR)/graph_4_detecting_cycles

graph_5_removing_nodes: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_5_removing_nodes -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_5_removing_nodes.c
	@./$(TEST_DIR)/graph_5_removing_nodes

graph_6_removing_edges: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/graph_6_removing_edges -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/graph_6_removing_edges.c
	@./$(TEST_DIR)/graph_6_removing_edges

deadlocks_1_basic_test_with_correct_locking_pattern_of_two_mutexes: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/deadlocks_1_basic_test_with_correct_locking_pattern_of_two_mutexes -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/deadlocks_1_basic_test_with_correct_locking_pattern_of_two_mutexes.c
	@./$(TEST_DIR)/deadlocks_1_basic_test_with_correct_locking_pattern_of_two_mutexes

deadlocks_2_basic_test_with_two_deadlocked_mutexes: $(ULTS_OBJ)
	@$(CC) -o $(TEST_DIR)/deadlocks_2_basic_test_with_two_deadlocked_mutexes -I$(INC_DIR) $(ULTS_OBJ) $(TEST_DIR)/deadlocks_2_basic_test_with_two_deadlocked_mutexes.c
	@./$(TEST_DIR)/deadlocks_2_basic_test_with_two_deadlocked_mutexes

run_tests: 1_basic_test 2_thread_joins_and_values_returned 3_expected_proper_exit 4_spawning_threads_recursively 5_verifying_thread_id_equality_check 6_verifying_thread_self 7_expected_proper_thread_cancellation 8_mutex_test 9_thread_yield_test 10_producer_consumer_test 11_threads_joining_each_other list_1_basic_test list_2_popping_elements list_3_removing_elements list_4_find_an_element graph_1_basic_test graph_2_count_nodes graph_3_count_edges graph_4_detecting_cycles graph_5_removing_nodes graph_6_removing_edges deadlocks_1_basic_test_with_correct_locking_pattern_of_two_mutexes deadlocks_2_basic_test_with_two_deadlocked_mutexes

copy:
	cp -f $(HEADER) $(INC_DIR)

clean:
	$(RM) -f *.o $(LIB_DIR)/* $(ULT_OBJ)
