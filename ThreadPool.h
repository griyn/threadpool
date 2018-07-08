#ifndef GRIYN_BASE_THREADPOOL_H
#define GRIYN_BASE_THREADPOOL_H

#include <pthread.h>
#include <queue>
#include <vector>

namespace griyn {
namespace base {

class Task;

class ThreadPool {
public:
    // 用工作者数量初始化线程池
    explicit ThreadPool(size_t worker_num);
    ~ThreadPool();

    // 运行线程池
    bool run();

    // 向工作线程池中投递任务
    bool post_task(Task*);

    // 
    Task* get_task();

    // 强制结束，并销毁线程池中的所有资源
    bool stop();

    // 任务队列。
    // 为了方便使用者自由控制任务数量，设为public
    bool is_task_empty();

private:
    std::vector<pthread_t> _workers;
    std::queue<Task*> _tasks;
    bool _is_working;

    // 唤醒线程使用的互斥锁
    pthread_mutex_t _mutex;

    // 工作者的工作内容
    static void* thread_work(void* param);
};

// 继承任务基类用于创建任务，是线程池任务队列的指定成员
class Task {
public:
    typedef void (*TASK_HANDLER)(void* param);
    explicit Task(TASK_HANDLER handler, void* param) :
        _handler(handler), 
        _param(param) {};
    ~Task() {};
    void handle() { _handler(_param); };

private:
    void* _param;
    TASK_HANDLER _handler;
};

/////////////////////类实现/////////////////////
ThreadPool::ThreadPool(size_t worker_num) : 
        _workers(worker_num),
        _is_working(true) {
    pthread_mutex_init(&_mutex, NULL);
}

ThreadPool::~ThreadPool() {
    stop();
}

bool ThreadPool::run() {
    for (int i = 0; i < _workers.size(); ++i)
        pthread_create(&_workers[i], NULL, thread_work, this);
    
    return true;
}

bool ThreadPool::post_task(Task * task) {
    pthread_mutex_lock(&_mutex);
    _tasks.push(task);
    pthread_mutex_unlock(&_mutex);
    return true;
}

bool ThreadPool::stop() {
    if (_is_working == false) {
        return true;
    }

    _is_working = false;

    for (size_t i = 0; i < _workers.size(); ++i) {
        void* status;
        pthread_join(_workers[i], &status);
    }

    pthread_mutex_destroy(&_mutex);

    return true;
}

Task* ThreadPool::get_task() {
    pthread_mutex_lock(&_mutex);

    Task* task = NULL;

    if (!_tasks.empty()) {
        task = _tasks.front();
        _tasks.pop(); 
    }

    pthread_mutex_unlock(&_mutex);

    return task;
}

void* ThreadPool::thread_work(void *param) {
    ThreadPool * p_thread_pool = (ThreadPool*)param;

    while (p_thread_pool->_is_working || !p_thread_pool->is_task_empty()) {
        Task* task = p_thread_pool->get_task();

        if (task) {
            task->handle();
        }
    }

    return NULL;
}

bool ThreadPool::is_task_empty() {
    return _tasks.empty();
}

} // base
} // griyn

#endif // GRIYN_BASE_THREADPOOL_H
