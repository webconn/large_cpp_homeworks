#include <iostream>
#include <memory>

class A {
 public:

   // ...

   static void* operator new(size_t size) {
     std::cout << "operator new!" << std::endl;
     return ::operator new(size);
   }

   static void operator delete(void* p, size_t size) {
     (void) size;

     std::cout << "operator delete!" << std::endl;
     return ::operator delete(p);
   }
};

template<class T, class U=T>
class MyAllocator {
 public:
  using value_type = T;
  using copied_from_type = U;

  MyAllocator() = default;
  MyAllocator(const MyAllocator<U>& u) {}

  T* allocate(size_t size) {
    return reinterpret_cast<T*>(copied_from_type::operator new(size * sizeof(T)));
  }

  void deallocate(T* p, std::size_t n) {
    copied_from_type::operator delete(p, n * sizeof(T));
  }
};

template<typename T, typename U>
bool operator==(const MyAllocator<T> &a, const MyAllocator<U> &b) { return true; }
template<typename T, typename U>
bool operator!=(const MyAllocator<T> &a, const MyAllocator<U> &b) { return false; }

int main() {
  auto my_allocator = MyAllocator<A>();
  auto sp = std::allocate_shared<A>(my_allocator);
}
