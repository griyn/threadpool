# threadpool
Simple C++ Thread pool 

Use pthread
Not used C++ 11 and boost

problem:

**If task function has few execution time**, we need time to create all thread. Otherwise `pthread_cond_broadcast` may not wake not created thread up. Then those threads will fall into endless loop. That's why `sleep(1)` here.