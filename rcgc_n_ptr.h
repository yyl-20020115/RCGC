#pragma once
#include "rcgc_base.h"

//rcgc_n_ptr is for something like std::wstring
//which you should not call detors but just
//decrease reference count.

template<class PTR>
class rcgc_n_ptr
    : public rcgc_base
{
public:
    static void terminating_function(void* ptr) {
        delete reinterpret_cast<PTR*>(ptr);
    }

public:

    rcgc_n_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    rcgc_n_ptr(const rcgc_n_ptr& src) :_ptr(src._ptr) {
        AddRef(this->_ptr, terminating_function);
    }
    ~rcgc_n_ptr() {
    }
public:
    void disposing() {

    }
    void finalize() {
        PTR* p = this->_ptr;
        if (p != nullptr) {
            this->_ptr = nullptr;
            //p->~PTR();
            RelRef(p);
        }
        OnCollecting<rcgc_n_ptr, PTR>(this);
    }
    rcgc_n_ptr& operator = (rcgc_n_ptr<PTR>& src) {
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
    PTR* rebind(PTR* p = nullptr) {
        PTR* op = this->_ptr;
        this->_ptr = p;
        return op;
    }
protected:
    PTR* _ptr;
};
