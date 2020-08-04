#pragma once
#include "rcgc_base.h"
//class PTR should have a method: void disposing()
template<class PTR>
class rcgc_d_ptr
    : public rcgc_base
{
public:
    static void terminating_function(void* ptr) {
        delete reinterpret_cast<PTR*>(ptr);
    }

public:

    rcgc_d_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    rcgc_d_ptr(const rcgc_d_ptr& src) :_ptr(src._ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    ~rcgc_d_ptr() {
        OnCollecting<rcgc_d_ptr,PTR>(this);
    }
public:
    void disposing(){
        PTR* p = this->_ptr;
        if (p != nullptr) {
            this->_ptr = nullptr;
            p->disposing();
            RelRef(p);
        }
    }
    rcgc_d_ptr& operator = (rcgc_d_ptr<PTR>& src) {
        if (this->_ptr == src._ptr) {
        }
        else if (src._ptr != nullptr) {
            RelRef(this->_ptr);
            AddRef(this->_ptr = src._ptr, terminating_function);
        }
        return *this;
    }
    operator bool() {
        return this->_ptr != nullptr;
    }
    PTR& operator*() const {
        return *this->_ptr;
    }
    PTR* operator->() const {
        return this->_ptr;
    }
    PTR* get() {
        return this->_ptr;
    }
    PTR* bind(PTR* p) {
        return this->_ptr = p;
    }
protected:
    PTR* _ptr;
};
