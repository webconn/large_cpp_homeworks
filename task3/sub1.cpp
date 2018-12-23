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
  std::condition_variable cv;

  std::thread producer([&]() {
    for (int i = 0; i < 10000; i++) {
      {
        std::lock_guard<std::mutex> g(items_mutex);
        items.push(i);
        count++;
      }
      cv.notify_one();
    }
    done = true;
  });

  std::thread consumer([&]() {
    while (!done) {
      std::unique_lock<std::mutex> ul(items_mutex);

      cv.wait_for(ul, 10ms);

      while (!items.empty()) {
        items.pop();
        // ...
        count--;
      }
    }
  });

  producer.join();
  consumer.join();

  std::cout << count << std::endl;

  return 0;
}
