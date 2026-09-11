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

