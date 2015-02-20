#ifndef UNDERSCORE_HPP
#define UNDERSCORE_HPP
#include <utility>

template<class T> struct _ {
	T *ptr;
	
	// Construct
	_() : ptr(nullptr) {}
	_(decltype(nullptr)) : ptr(nullptr) {}
	_(T *t) : ptr(t) {}
	template<typename... Args> _(Args&&... args) {
		ptr = new T(std::forward<Args>(args)...);
	}
	
	// No copy
	_(const _&) = delete;
	_& operator=(const _&) = delete;
	
	// Move
	_(_ &&o) {
		delete ptr;
		ptr = o.ptr;
		o.ptr = nullptr;
	}
	_& operator=(_ &&o) {
		delete ptr;
		ptr = o.ptr;
		o.ptr = nullptr;
		return *this;
	}
	
	// Assign
	_& operator=(T *t) {
		delete ptr;
		ptr = t;
		return *this;
	}
	
	// Comparison
	bool operator==(T *t) const {
		return t == ptr;
	}
	bool operator!=(T *t) const {
		return t != ptr;
	}
	
	// Get
	T* get() const {
		return ptr;
	}
	T& operator[](int i) const {
		return ptr[i];
	}
	T& operator*() const {
		return *ptr;
	}
	T* operator->() const {
		return ptr;
	}
	
	// Addressof, use at your own risk
	T** operator&() const {
		return &ptr;
	}
	
	// Cast
	operator T*() const {
		return ptr;
	}
	operator const T*() const {
		return ptr;
	}
	template<typename R> operator R*() const {
		return (R*)ptr;
	}
	operator bool() const {
		return ptr;
	}
	
	// Destruct
	~_() {
		delete ptr;
	}
};

#endif