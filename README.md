# threads-project

How do multiple types of threads yield a processor?

- preemptive threads
  - 1:1 mapping to system threads &rarr; native system threads
  - n:1 mapping to system threads (what we will implement) &rarr; green thread
  - n:n mapping to system threads &rarr; goroutines and Java virtual threads

- cooperative threads (needs to yield processor through an explicit await)
  - 1:1 mapping to system threads &rarr; fibers and coroutines
  - n:1 mapping to system threads (what we will implement)
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
