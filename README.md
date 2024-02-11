# threads-project

An implementation of a thread library using a preemptive scheduling mechanism (round-robin) and a deadlock detection algorithm.

## Notes

What is __round-robin scheduling__?

__Round-robin__ is a preemptive scheduling algorithm where time slices are assigned to each process in equal portions and in circular order. The processes are not ordered by priority and whenever a process does not finish its job in a given time slice, it is placed into the queue to be resumed later on. In other words, the job is suspended after the duration of the time slice and is pushed onto the queue if it is not yet completed.

What is the __shortest-remaining time first__ algorithm?

Shortest Remaining Time First is a preemptive CPU scheduling algorithm that selects the process with the smallest remaining burst time to execute next. It continuously monitors the remaining execution time of processes and may preempt the currently running process if a new process arrives with a shorter remaining time.

How does the waits-for-graph deadlock detection algorithm?

The waits-for graph is a directed graph representation of the resource allocation and resource request relationships among processes. In this graph, nodes represent either processes or resources, while edges represent the relationship between processes and resources. If a process is currently holding a resource, there is a directed edge from the process to the resource. If a process is waiting for a resource, there is a directed edge from the resource to the process.

1. Initialize the waits-for graph with nodes representing processes and resources, and edges representing the allocation and request relationships.
2. Periodically scan the graph to identify cycles. A cycle in the graph indicates a potential deadlock.
3. If a cycle is detected, determine whether it corresponds to a deadlock by checking whether each process in the cycle is waiting for a resource held by the next process in the cycle.
4. If all processes in the cycle are indeed waiting for a resource held by the next process in the cycle, then a deadlock is confirmed.
5. Once a deadlock is detected, appropriate actions can be taken to resolve it, such as preemptively releasing resources held by processes or killing processes to break the deadlock.

How do multiple types of threads yield a processor?

- preemptive threads
  - 1:1 mapping to system threads &rarr; native system threads
  - n:1 mapping to system threads (what we will implement) &rarr; green thread
  - n:n mapping to system threads &rarr; goroutines and Java virtual threads

- cooperative threads (needs to yield processor through an explicit await)
  - 1:1 mapping to system threads &rarr; fibers and coroutines
  - n:1 mapping to system threads
  - n:n mapping to system threads

- functions for the thread library project
  - `get_context` &rarr; used for `user_level_thread.create()` from the example
  - `make_context` &rarr; provide the function for which you create a context (context of a thread)
  - `swap_context` &rarr; swaps contexts for functions
  - signals (for preemption - `alarm` function, requires waiting multiple seconds...) &rarr; solutions: `vtalarm`, `sig_mask`, signals issued at a regular time interval to trigger the scheduler to give processor to other threads 
    - signals can disrupt the state of a function

- chaining contexts
- you can link a context from another context (context finishes meaning that the function ends) &rarr; `next_context`
- essentially, the thread library is a scheduler
- context per thread
- "linked list of function contexts"
- how do you implement `pthread_join` or basically make a thread wait and let the processor perform other operations? &rarr; threads are marked as "to be waited" and no other thread is executed before the marked ones are done executing

- preemptive &rarr; managed by the scheduler
- cooperative &rarr; not managed by the scheduler

```
struct ult_t {
  int id;
  contexts;
  .
  .
  .
}

ult_init(); // to avoid syntactic hell, responsible for making the main function a "controllable" thread

ult_create();

ult_join();

ult_yield();

ult_exit();

ult_self(); // returns the id (thread id?)
```

- round robin scheduler
