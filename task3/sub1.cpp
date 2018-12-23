#include <iostream>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std::chrono_literals;

int main() {
  size_t count = 0;
  std::atomic_bool done(false);

  std::queue<int> items;
  std::mutex items_mutex;
  std::condition_variable push_cv, pop_cv;

  std::thread producer([&]() {
    for (int i = 0; i < 100000; i++) {
      {
        std::lock_guard<std::mutex> g(items_mutex);
        items.push(i);
        count++;
      }
      push_cv.notify_all();
    }

    bool all_recvd = false;
    while (!all_recvd) {
      std::unique_lock<std::mutex> ul(items_mutex);
      pop_cv.wait_for(ul, 1ms);

      if (count == 0) {
        done = true;
        all_recvd = true;
      }
    }
  });

  std::thread consumer([&]() {
    while (!done) {
      std::unique_lock<std::mutex> ul(items_mutex);

      push_cv.wait_for(ul, 1ms);

      while (!items.empty()) {
        items.pop();
        // ...
        count--;
      }

      pop_cv.notify_one();
    }
  });

  producer.join();
  consumer.join();

  std::cout << count << std::endl;

  return 0;
}
