#include <iostream>
#include <cassert>

template<typename T>
class TBasePtr {
  public:
    using value_type = T;

    TBasePtr(T* ptr) : ptr_(ptr) {}

    T* operator->() const {
      return ptr_;
    }

    T& operator*() const {
      return *ptr_;
    }

    bool operator==(const TBasePtr& p) const {
      return ptr_ == p.ptr_;
    }

    bool operator!=(const TBasePtr& p) const {
      return ptr_ != p.ptr_;
    }

    operator bool() const {
      return ptr_ != nullptr;
    }

  protected:
    T* ptr_;
};

template<typename T>
class TIntrusivePtr : public TBasePtr<T> {
  public:
    TIntrusivePtr(T* ptr) : TBasePtr<T>(ptr) {
      Get()->AddRef();
    }

    TIntrusivePtr(const TIntrusivePtr& p) : TBasePtr<T>(p.ptr_) {
      Get()->AddRef();
    }

    TIntrusivePtr(TIntrusivePtr&& p) : TBasePtr<T>(p.ptr_) {
      p.ptr_ = nullptr;
    }

    ~TIntrusivePtr() {
      Release();
      if (UseCount() == 0) {
        delete TBasePtr<T>::ptr_;
      }
    }

    T* Get() const {
      return TBasePtr<T>::ptr_;
    }

    unsigned UseCount() const {
      return Get()->UseCount();
    }

    void Reset() {
      Release();
      TBasePtr<T>::ptr_ = nullptr;
    }

    void Reset(T* ptr) {
      Get()->Release();
      TBasePtr<T>::ptr_ = ptr;
      Get()->AddRef();
    }

    void Release() const {
      Get()->Release();
    }
};

template<typename T>
class TRefCounter {
  public:
    void AddRef() {
      ref_counter_++;
    }

    void Release() {
      ref_counter_--;
    }

    unsigned UseCount() const {
      return ref_counter_;
    }

  private:
    unsigned ref_counter_ = 0;
};

class TDoc : public TRefCounter<TDoc> {
  public:
    TDoc(int a) : a_(a) {}

    int GetA() const { return a_; }
    void SetA(int a) { a_ = a; }

  private:
    int a_;
};

int main() {
  TDoc* p = new TDoc(1);

  TIntrusivePtr<TDoc> ip1(p);

  {
    TIntrusivePtr<TDoc> ip2(p);
    assert(ip1->UseCount() == 2);
  }

  assert(ip1->UseCount() == 1);

  return 0;
}
