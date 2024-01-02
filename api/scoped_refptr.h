/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// Originally these classes are from Chromium.
// http://src.chromium.org/viewvc/chrome/trunk/src/base/memory/ref_counted.h?view=markup

//
// A smart pointer class for reference counted objects.  Use this class instead
// of calling AddRef and Release manually on a reference counted object to
// avoid common memory leaks caused by forgetting to Release an object
// reference.  Sample usage:
//
//   class MyFoo : public RefCounted<MyFoo> {
//    ...
//   };
//
//   void some_function() {
//     scoped_refptr<MyFoo> foo = new MyFoo();
//     foo->Method(param);
//     // |foo| is released when this function returns
//   }
//
//   void some_other_function() {
//     scoped_refptr<MyFoo> foo = new MyFoo();
//     ...
//     foo = nullptr;  // explicitly releases |foo|
//     ...
//     if (foo)
//       foo->Method(param);
//   }
//
// The above examples show how scoped_refptr<T> acts like a pointer to T.
// Given two scoped_refptr<T> classes, it is also possible to exchange
// references between the two objects, like so:
//
//   {
//     scoped_refptr<MyFoo> a = new MyFoo();
//     scoped_refptr<MyFoo> b;
//
//     b.swap(a);
//     // now, |b| references the MyFoo object, and |a| references null.
//   }
//
// To make both |a| and |b| in the above example reference the same MyFoo
// object, simply use the assignment operator:
//
//   {
//     scoped_refptr<MyFoo> a = new MyFoo();
//     scoped_refptr<MyFoo> b;
//
//     b = a;
//     // now, |a| and |b| each own a reference to the same MyFoo object.
//   }
//

#ifndef API_SCOPED_REFPTR_H_
#define API_SCOPED_REFPTR_H_

#include <memory>
#include <utility>

namespace rtc {

//用于管理引用计数的对象，它的主要目的是在对象的生命周期内自动管理引用计数，避免内存泄漏
template <class T>
class scoped_refptr {
 public:
  typedef T element_type;

  //默认构造函数，将ptr初始化为nullptr
  scoped_refptr() : ptr_(nullptr) {}

  //带有指针的构造函数，接收一个指向引用计数对象的指针，如果ptr不为空，则增加引用计数
  scoped_refptr(T* p) : ptr_(p) {  // NOLINT(runtime/explicit)
    if (ptr_)
      ptr_->AddRef();
  }

  //拷贝构造函数，接收另一个scoped_refptr 对象 增加引用计数
  scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_) {
    if (ptr_)
      ptr_->AddRef();
  }

  //拷贝构造函数的模板版本，允许从不同类型的 scoped_refptr 对象进行拷贝构造。
  template <typename U>
  scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get()) {
    if (ptr_)
      ptr_->AddRef();
  }

  // Move constructors.
  //移动构造函数，接收一个右值引用，并通过 release() 来获取其指针。该构造函数将 r 中的指针接管，同时将 r 的指针置为 nullptr。
  scoped_refptr(scoped_refptr<T>&& r) noexcept : ptr_(r.release()) {}

  template <typename U>
  scoped_refptr(scoped_refptr<U>&& r) noexcept : ptr_(r.release()) {}

  //析构函数，释放持有的对象。如果 ptr_ 不为空，则调用对象的 Release() 方法。
  ~scoped_refptr() {
    if (ptr_)
      ptr_->Release();
  }

  T* get() const { return ptr_; }
  operator T*() const { return ptr_; }
  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }

  // Returns the (possibly null) raw pointer, and makes the scoped_refptr hold a
  // null pointer, all without touching the reference count of the underlying
  // pointed-to object. The object is still reference counted, and the caller of
  // release() is now the proud owner of one reference, so it is responsible for
  // calling Release() once on the object when no longer using it.
  //释放 scoped_refptr 对象对引用计数对象的所有权，返回指向该对象的指针，并将自身的指针置为 nullptr。
  T* release() {
    T* retVal = ptr_;
    ptr_ = nullptr;
    return retVal;
  }

  scoped_refptr<T>& operator=(T* p) {
    // AddRef first so that self assignment should work
    if (p)
      p->AddRef();
    if (ptr_)
      ptr_->Release();
    ptr_ = p;
    return *this;
  }

  scoped_refptr<T>& operator=(const scoped_refptr<T>& r) {
    return *this = r.ptr_;
  }

  template <typename U>
  scoped_refptr<T>& operator=(const scoped_refptr<U>& r) {
    return *this = r.get();
  }

  scoped_refptr<T>& operator=(scoped_refptr<T>&& r) noexcept {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  template <typename U>
  scoped_refptr<T>& operator=(scoped_refptr<U>&& r) noexcept {
    scoped_refptr<T>(std::move(r)).swap(*this);
    return *this;
  }

  void swap(T** pp) noexcept {
    T* p = ptr_;
    ptr_ = *pp;
    *pp = p;
  }

  //交换函数，用于在对象之间交换引用计数对象的指针
  void swap(scoped_refptr<T>& r) noexcept { swap(&r.ptr_); }

 protected:
  T* ptr_;
};

}  // namespace rtc

#endif  // API_SCOPED_REFPTR_H_
