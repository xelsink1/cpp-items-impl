#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <cstdlib>

template<class T>
class Queue {
public:
    void Push(T item) {
        {
            std::lock_guard<std::mutex> lock(m);
            if (closed_) throw std::exception();
            q.push(std::move(item));
        }
        cv.notify_one();
    }

    std::optional<T> Pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this] {
            return !q.empty() || closed_;
        });

        if (closed_ && q.empty())
            return std::nullopt;

        auto item = std::move(q.front());
        q.pop();
        cv.notify_all();
        return item;
    }

    void Close() {
        {
            std::lock_guard<std::mutex> lock(m);
            closed_ = true;
        }
        cv.notify_all();
    }

private:
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool closed_ = false;
};

std::mutex cout_mutex;

void producer(Queue<int>& q) {
    for (int i = 0; i < 100; i++) {
        if (std::rand() % 5 <= 3) {
            int qp = std::rand() % 10 + 1;
            q.Push(qp);
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Добавил работу " << qp << '\n';
            }
        }
    }
    q.Close();
}

void consumer(Queue<int>& q) {
    while (true) {
        auto job = q.Pop();
        if (!job) break;
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Работа " << *job << " выполнена!" << '\n';
        }
    }
}
