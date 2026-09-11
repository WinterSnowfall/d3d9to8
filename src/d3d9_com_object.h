#pragma once

#include <atomic>

template<typename BaseObject>
class ComObjectClamp : public BaseObject {

public:

  virtual ~ComObjectClamp() { }

  ULONG STDMETHODCALLTYPE AddRef() {
    ULONG refCount = m_refCount++;
    return refCount + 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    ULONG refCount = this->m_refCount;
    if (refCount != 0ul) {
      this->m_refCount--;
      refCount--;

      if (refCount == 0ul)
        delete this;
    }

    return refCount;
  }

  BaseObject* IncrementRef() {
    this->AddRef();
    return this;
  }

protected:

  std::atomic<ULONG> m_refCount = { 0ul };

};

template<typename T>
class ComObject {

public:

  ComObject() { }
  ComObject(std::nullptr_t) { }
  ComObject(T* object)
  : m_ptr(object) {
    if (m_ptr != nullptr)
      m_ptr->AddRef();
  }

  ComObject(const ComObject& other)
  : m_ptr(other.m_ptr) {
    if (m_ptr != nullptr)
      m_ptr->AddRef();
  }

  ComObject(ComObject&& other)
  : m_ptr(other.m_ptr) {
    other.m_ptr = nullptr;
  }

  ComObject& operator = (T* object) {
    if (m_ptr != object) {
      if (m_ptr != nullptr)
        m_ptr->Release();
      m_ptr = object;
      if (m_ptr != nullptr)
        m_ptr->AddRef();
    }
    return *this;
  }

  ComObject& operator = (const ComObject& other) {
    if (other != nullptr)
      other.ref();
    if (m_ptr != nullptr)
      m_ptr->Release();
    m_ptr = other.m_ptr;
    return *this;
  }

  ComObject& operator = (ComObject&& other) {
    if (m_ptr != nullptr)
      m_ptr->Release();
    this->m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
  }

  ComObject& operator = (std::nullptr_t) {
    if (m_ptr != nullptr) {
      m_ptr->Release();
      m_ptr = nullptr;
    }
    return *this;
  }

  ~ComObject() {
    if (m_ptr != nullptr) {
      m_ptr->Release();
      m_ptr = nullptr;
    }
  }

  T* operator -> () const {
    return m_ptr;
  }

  T**       operator & ()       { return &m_ptr; }
  T* const* operator & () const { return &m_ptr; }

  bool operator == (const ComObject<T>& other) const { return m_ptr == other.m_ptr; }
  bool operator != (const ComObject<T>& other) const { return m_ptr != other.m_ptr; }

  bool operator == (const T* other) const { return m_ptr == other; }
  bool operator != (const T* other) const { return m_ptr != other; }

  bool operator == (std::nullptr_t) const { return m_ptr == nullptr; }
  bool operator != (std::nullptr_t) const { return m_ptr != nullptr; }

  T* ref() const {
    if (m_ptr != nullptr)
      m_ptr->AddRef();
    return m_ptr;
  }

  T* ptr() const {
    return m_ptr;
  }

  explicit operator bool () const {
    return m_ptr != nullptr;
  }

private:

  T* m_ptr = nullptr;

};

