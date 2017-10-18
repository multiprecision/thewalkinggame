// thread_pool.h

#ifndef OGLPROJ_THREAD_POOL_H_
#define OGLPROJ_THREAD_POOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <queue>

namespace oglproj
{

class Thread_pool
{
public:
    Thread_pool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    ->std::future < typename std::result_of<F(Args...)>::type > ;
    ~Thread_pool();
private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> m_workers;
    // the task queue
    std::queue<std::function<void()> > m_tasks;
    // synchronization
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

// the constructor just launches some amount of workers
inline Thread_pool::Thread_pool(size_t threads)
    : m_stop(false)
{
    for (size_t i = 0; i < threads; i++)
        m_workers.emplace_back(
            [this]
    {
        for (;;)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                this->m_condition.wait(lock,
                [this] { return this->m_stop || !this->m_tasks.empty(); });
                if (this->m_stop && this->m_tasks.empty())
                    return;
                task = std::move(this->m_tasks.front());
                this->m_tasks.pop();
            }

            task();
        }
    }
    );
}

// add new work item to the pool
template<class F, class... Args>
auto Thread_pool::enqueue(F&& f, Args&&... args)
-> std::future < typename std::result_of<F(Args...)>::type >
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (m_stop)
            throw std::runtime_error("enqueue on stopped thread pool");

        m_tasks.emplace([task]()
        {
            (*task)();
        });
    }
    m_condition.notify_one();
    return res;
}

// the destructor joins all threads
inline Thread_pool::~Thread_pool()
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for (std::thread &worker : m_workers)
        worker.join();
}


} // end of namespace oglproj

#endif