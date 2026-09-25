#pragma once

#include <atomic>

template<typename BaseObject>
class ComObjectClamp : public BaseObject {

public:

  virtual ~ComObjectClamp() { }

  ULONG STDMETHODCALLTYPE AddRef() {
    uint32_t refCount = m_refCount++;
    if (!refCount)
      AddRefPrivate();
    return refCount + 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    uint32_t refCount = this->m_refCount;
    if (refCount != 0ul) {
      this->m_refCount--;
      refCount--;

      if (refCount == 0ul)
        this->ReleasePrivate();
    }

    return refCount;
  }

  void AddRefPrivate() {
    ++m_refPrivate;
  }

  void ReleasePrivate() {
    uint32_t refPrivate = --m_refPrivate;
    if (!refPrivate) {
      m_refPrivate += 0x80000000;
      delete this;
    }
  }

protected:

  std::atomic<uint32_t> m_refCount = { 0ul };
  std::atomic<uint32_t> m_refPrivate = { 0ul };

};

template<typename T>
inline void ClearReturnPointer(T** ptr) {
  if (ptr != nullptr)
    *ptr = nullptr;
}

template<typename T>
T* ref(T* object) {
  if (object != nullptr)
    object->AddRef();
  return object;
}

template<typename T, bool Public>
struct ComRef_ {
  static void incRef(T* ptr) { ptr->AddRef(); }
  static void decRef(T* ptr) { ptr->Release(); }
};


template<typename T>
struct ComRef_<T, false> {
  static void incRef(T* ptr) { ptr->AddRefPrivate(); }
  static void decRef(T* ptr) { ptr->ReleasePrivate(); }
};

template<typename T, bool Public = true>
class ComObject {

public:

  ComObject() { }
  ComObject(std::nullptr_t) { }
  ComObject(T* object)
  : m_ptr(object) {
    this->incRef();
  }

  ComObject(const ComObject& other)
  : m_ptr(other.m_ptr) {
    this->incRef();
  }

  ComObject(ComObject&& other)
  : m_ptr(other.m_ptr) {
    other.m_ptr = nullptr;
  }

  ComObject& operator = (T* object) {
    if (likely(m_ptr != object)) {
      this->decRef();
      m_ptr = object;
      this->incRef();
    }
    return *this;
  }

  ComObject& operator = (const ComObject& other) {
    other.incRef();
    this->decRef();
    m_ptr = other.m_ptr;
    return *this;
  }

  ComObject& operator = (ComObject&& other) {
    this->decRef();
    this->m_ptr = other.m_ptr;
    other.m_ptr = nullptr;
    return *this;
  }

  ComObject& operator = (std::nullptr_t) {
    this->decRef();
    m_ptr = nullptr;
    return *this;
  }

  ~ComObject() {
    this->decRef();
    m_ptr = nullptr;
  }

  T* operator -> () const {
    return m_ptr;
  }

  T**       operator & ()       { return &m_ptr; }
  T* const* operator & () const { return &m_ptr; }

  template<bool Public_>
  bool operator == (const ComObject<T, Public_>& other) const { return m_ptr == other.m_ptr; }
  template<bool Public_>
  bool operator != (const ComObject<T, Public_>& other) const { return m_ptr != other.m_ptr; }

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

  ComObject<T, true>  pubRef() const { return m_ptr; }
  ComObject<T, false> prvRef() const { return m_ptr; }

  explicit operator bool () const {
    return m_ptr != nullptr;
  }

private:

  T* m_ptr = nullptr;

  void incRef() const {
    if (m_ptr != nullptr)
      ComRef_<T, Public>::incRef(m_ptr);
  }

  void decRef() const {
    if (m_ptr != nullptr)
      ComRef_<T, Public>::decRef(m_ptr);
  }

};
