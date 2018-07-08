#include <iostream>
#include <unistd.h>
#include "ThreadPool.h"

void func1(void* param) {
    std::cout << "hello" << std::endl;
}

void func2(void* param) {
    std::cout << "world" << std::endl;
}

int main() {
    griyn::base::ThreadPool pool(5);
    pool.run();
    pool.post_task(new griyn::base::Task(func1, NULL));
    pool.post_task(new griyn::base::Task(func2, NULL));
    pool.post_task(new griyn::base::Task(func1, NULL));
    pool.post_task(new griyn::base::Task(func2, NULL));    
    pool.post_task(new griyn::base::Task(func1, NULL));
    pool.post_task(new griyn::base::Task(func2, NULL));    
    pool.post_task(new griyn::base::Task(func1, NULL));
    pool.post_task(new griyn::base::Task(func2, NULL));
    return 0;
}