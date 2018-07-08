#include <iostream>
#include <unistd.h>
#include "ThreadPool.h"

void func1(void* param) {
    std::cout << "hello1" << std::endl;
}

void func2(void* param) {
    std::cout << "world2" << std::endl;
}

int main() {
    griyn::base::ThreadPool pool(5);
    pool.run();
    pool.post_task(new griyn::base::Task(func1, NULL));
    pool.post_task(new griyn::base::Task(func2, NULL));
    sleep(1);
    return 0;
}