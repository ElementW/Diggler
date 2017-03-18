#ifndef DIGGLER_UTIL_UNIQUE_FUNCTION_HPP
#define DIGGLER_UTIL_UNIQUE_FUNCTION_HPP

#include <memory>
#include <utility>

namespace Diggler {
namespace Util {

template <typename T>
class unique_function;

template<typename R, typename ...Args>
class unique_function<R(Args...)> {
  using InvokeFnT = R (*)(char*, Args&&...);
  using DestroyFnT = void (*)(char*);

  template <typename Functor>
  static R InvokeFn(Functor *fn, Args&&... args) {
      return (*fn)(std::forward<Args>(args)...);
  }

  template <typename Functor>
  static void DestroyFn(Functor* f) {
      f->~Functor();
  }

  InvokeFnT invokeFn;
  DestroyFnT destroyFn;
  std::unique_ptr<char[]> data;
  size_t dataSize;
public:
  unique_function() :
    invokeFn(nullptr),
    destroyFn(nullptr),
    data(nullptr),
    dataSize(0) {
  }

  unique_function(decltype(nullptr)) :
    unique_function() {
  }

  template <typename Functor>
  unique_function(Functor &&f) :
    invokeFn(reinterpret_cast<InvokeFnT>(InvokeFn<Functor>)),
    destroyFn(reinterpret_cast<DestroyFnT>(DestroyFn<Functor>)),
    data(new char[sizeof(Functor)]),
    dataSize(sizeof(Functor)) {
    new (data.get()) Functor(std::move(f));
  }

  unique_function(const unique_function&) = delete;
  unique_function& operator=(const unique_function&) = delete;

  unique_function(unique_function &&o) :
    unique_function() {
    operator=(std::move(o));
  }

  unique_function& operator=(unique_function &&o) {
    invokeFn = o.invokeFn;
    // Safeguard; will trigger a segfault if operator() is called on a moved unique_function
    o.invokeFn = nullptr;
    destroyFn = o.destroyFn;
    data = std::move(o.data);
    dataSize = o.dataSize;
    return *this;
  }

  ~unique_function() {
    if (data) {
      destroyFn(data.get());
    }
  }

  R operator()(Args&&... args) {
    return invokeFn(data.get(), std::forward<Args>(args)...);
  }

  void swap(unique_function<R(Args...)> &o) {
    using std::swap;
    swap(invokeFn, o.invokeFn);
    swap(destroyFn, o.destroyFn);
    swap(data, o.data);
    swap(dataSize, o.dataSize);
  }
};

template <typename T>
void swap(unique_function<T> &f1, unique_function<T> &f2) {
  f1.swap(f2);
}

}
}

#endif /* DIGGLER_UTIL_UNIQUE_FUNCTION_HPP */
