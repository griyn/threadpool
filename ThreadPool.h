#ifndef GRIYN_BASE_THREADPOOL_H
#define GRIYN_BASE_THREADPOOL_H

#include <pthread.h>
#include <queue>
#include <vector>

namespace griyn {
namespace base {

class ThreadPool {
public:
    // 用工作者数量初始化线程池
    explicit ThreadPool(size_t worker_num);
    ~ThreadPool();

    // 运行线程池
    bool run();

    // 向工作线程池中投递任务
    bool post_task(Task*);

    // 强制结束，并销毁线程池中的所有资源
    bool stop();

    // 任务队列。
    // 为了方便使用者自由控制任务数量，设为public
    std::queue<TaskBase*> _tasks;

private:
    std::vector<pthread_t> _workers;
    bool _is_working;

    // 唤醒线程使用的互斥锁
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    bool cond_wait();

    // 工作者的工作内容
    static void* thread_work(void* param);
};

// 继承任务基类用于创建任务，是线程池任务队列的指定成员
class TaskBase {
public:
    explicit TaskBase(void* param) : _param(param);
    virtual ~TaskBase() {};

    virtual void handle() = 0;

private:
    void* _param;
};

/////////////////////类实现/////////////////////
ThreadPool::ThreadPool(size_t worker_num) : 
        _workers(worker_num)
        _is_working(true) {
    pthread_mutex_init(_mutex, NULL);
    pthread_cond_init(_cond, NULL);
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
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_mutex);
    return true;
}

bool ThreadPool::wait() {
    
}

bool ThreadPool::stop() {
    is_working = false;
    pthread_cond_broadcast(&_cond);

    if (!_tasks.empty()) {
        delete _tasks.front();
        _tasks.pop();
    }

    for (size_t i = 0; i < _workers.size(); ++i) {
        void* status;
        pthread_join(_workers[i], &status);
    }

    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);

    return true;
}

bool ThreadPool::wait_for_task() {
    pthread_mutex_lock(&_mutex);
    pthread_cond_wait(&_cond, &_mutex);
    pthread_mutex_unlock(&_mutex);
}

void *ThreadPool::work_content(void *param) {
    ThreadPool * p_thread_pool = (ThreadPool*)param;

    while (p_thread_pool->is_working || p_thread_pool->_tasks.empty()) {
        p_thread_pool->wait_for_task();

        Task* task = threadPool->_tasks.front();
        threadPool->_tasks.pop();
        task->handle();
    }

    return nullptr;
}

} // base
} // griyn

#endif // GRIYN_BASE_THREADPOOL_H
