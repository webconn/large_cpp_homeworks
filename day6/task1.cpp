#include <iostream>
#include <vector>

template<typename T>
class MyAllocator {
  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;

    pointer allocate(size_t n) {
      bool use_heap = true;
      unsigned static_offset = 0;


      if (buffer_used <= n) {
        size_t left = n;

        for (unsigned i = 0; i < buffer_size; i++) {
          if (left == n && elems_used[i] == false) {
            left--;
            static_offset = i;
          } else if (left > 0 && elems_used[i] == false) {
            left--;
          } else if (left > 0 && elems_used[i] == true) {
            left = n;
          }

          if (left == 0) {
            use_heap = false;
            break;
          }
        }
      }

      if (use_heap) {
        std::cout << "Allocated " << n << " elements in heap" << std::endl;
        return static_cast<pointer>(::operator new(n));
      } else {
        std::cout << "Allocated " << n << " elements in static buffer" << std::endl;
        for (unsigned i = 0; i < n; i++) {
          elems_used[static_offset + i] = true;
          buffer_used++;
        }
        return &buffer[static_offset];
      }
    }

    void deallocate(pointer p, size_t n) {
      if (p >= buffer && p <= &buffer[buffer_size - 1]) {
        std::cout << "Freed " << n << " elements from static buffer" << std::endl;
        const unsigned start = p - buffer;
        for (unsigned i = 0; i < n; i++) {
          elems_used[start + i] = false;
          buffer_used--;
        }
      } else {
        std::cout << "Freed " << n << " elements from heap" << std::endl;
        return ::operator delete(p);
      }
    }
  private:
    static constexpr unsigned buffer_size = 16;
    static value_type buffer[buffer_size];
    static bool elems_used[buffer_size];
    static size_t buffer_used;
};

template<>
int MyAllocator<int>::buffer[MyAllocator<int>::buffer_size] = {};
template<>
bool MyAllocator<int>::elems_used[MyAllocator<int>::buffer_size] = {};
template<>
size_t MyAllocator<int>::buffer_used = 0;

int main() {

  std::vector<int, MyAllocator<int>> a;

  a.push_back(1);
  a.push_back(2);
  a.push_back(3);
  a.push_back(4);

  a.reserve(32);

  return 0;
}
