#pragma once
#include "rcgc_base.h"
//class PTR should have a method: void disposing() and void finalize()
//the finalize() is used to be called once before free(ptr) 
template<class PTR>
class rcgc_f_ptr
    : public rcgc_base
{
public:
    static void terminating_function(void* ptr) {
        reinterpret_cast<PTR*>(ptr)->finalize();
        free(ptr);
    }

public:

    rcgc_f_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    rcgc_f_ptr(const rcgc_f_ptr& src) :_ptr(src._ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    ~rcgc_f_ptr() {
        OnCollecting<rcgc_f_ptr,PTR>(this);
    }
public:
    void disposing() {
        PTR* p = this->_ptr;
        if (p != nullptr) {
            this->_ptr = nullptr;
            p->~PTR();
            RelRef(p);
        }
    }
    rcgc_f_ptr& operator = (rcgc_f_ptr<PTR>& src) {
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
