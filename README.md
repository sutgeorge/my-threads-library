# threads-project

An implementation of a thread library using a preemptive scheduling mechanism (round-robin) and a deadlock detection algorithm.

## Notes

- What is __round-robin scheduling__?

__Round-robin__ is a preemptive scheduling algorithm where time slices are assigned to each process in equal portions and in circular order. The processes are not ordered by priority and whenever a process does not finish its job in a given time slice, it is placed into the queue to be resumed later on. In other words, the job is suspended after the duration of the time slice and is pushed onto the queue if it is not yet completed.

- What is the __shortest-remaining time first__ algorithm?

Shortest Remaining Time First is a preemptive CPU scheduling algorithm that selects the process with the smallest remaining burst time to execute next. It continuously monitors the remaining execution time of processes and may preempt the currently running process if a new process arrives with a shorter remaining time.

- How does the waits-for-graph deadlock detection algorithm?

The waits-for graph is a directed graph representation of the resource allocation and resource request relationships among processes. In this graph, nodes represent either processes or resources, while edges represent the relationship between processes and resources. If a process is currently holding a resource, there is a directed edge from the process to the resource. If a process is waiting for a resource, there is a directed edge from the resource to the process.

1. Initialize the waits-for graph with nodes representing processes and resources, and edges representing the allocation and request relationships.
2. Periodically scan the graph to identify cycles. A cycle in the graph indicates a potential deadlock.
3. If a cycle is detected, determine whether it corresponds to a deadlock by checking whether each process in the cycle is waiting for a resource held by the next process in the cycle.
4. If all processes in the cycle are indeed waiting for a resource held by the next process in the cycle, then a deadlock is confirmed.
5. Once a deadlock is detected, appropriate actions can be taken to resolve it, such as preemptively releasing resources held by processes or killing processes to break the deadlock.

- What are signal sets?

Signal sets are data objects that let a process keep track of groups of signals. For example, a process can create one signal set to record which signals it is blocking, and another signal set to record which signals are pending.

- Why is the SIGVTALARM signal is blocked and unblocked in several functions of the library? 

This is done so that operations can be performed without any interruption caused by the SIGVTALARM signal.

- Knowing that the SIGVTALARM signal is blocked and unblocked in several functions of the library, [what is the actual difference between a signal and an interrupt from ASM](https://stackoverflow.com/questions/13341870/signals-and-interrupts-a-comparison/13380714#13380714)? 

Interrupts can be viewed as a mean of communication between the CPU and the OS kernel. Signals can be viewed as a mean of communication between the OS kernel and OS processes.

Interrupts may be initiated by the CPU (exceptions - e.g.: divide by zero, page fault), devices (hardware interrupts - e.g: input available), or by a CPU instruction (traps - e.g: syscalls, breakpoints). They are eventually managed by the CPU, which "interrupts" the current task, and invokes an OS-kernel provided ISR/interrupt handler.

Signals may be initiated by the OS kernel (e.g: SIGFPE, SIGSEGV, SIGIO), or by a process(kill()). They are eventually managed by the OS kernel, which delivers them to the target thread/process, invoking either a generic action (ignore, terminate, terminate and dump core) or a process-provided signal handler.

- What is the type of `pthread_t` and what type should we use for the green thread/user-level library?

The type is `unsigned long int`, but the POSIX standard specifies that `pthread_t` could be represented by a more complex structure. It is convenient to have `pthread_t` as a data type other than an `unsigned long int` [on systems that do not support such a data type](https://stackoverflow.com/questions/33285562/why-and-in-what-sense-is-pthread-t-an-opaque-type). However, we will use the `unsigned long int` in the project. 

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
