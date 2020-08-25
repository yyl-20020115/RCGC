#pragma once
#include "rcgc_base.h"

template<class PTR>
class rcgc_ptr
    : public rcgc_base
{
public:

    rcgc_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr);
    }
    rcgc_ptr(const rcgc_ptr& src) :_ptr(src._ptr){
        AddRef(this->_ptr);
    }
    ~rcgc_ptr() {
        OnCollecting<rcgc_ptr,PTR>(this);
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
    rcgc_ptr& operator = (rcgc_ptr<PTR>& src) {
        if (this->_ptr == src._ptr) {
        }
        else if (src._ptr != nullptr) {
            RelRef(this->_ptr);
            AddRef(this->_ptr = src._ptr);
        }
        return *this;
    }
    operator bool() const {
        return this->_ptr != nullptr;
    }
    PTR& operator*() const {
        return *this->_ptr;
    }
    PTR* operator->() const {
        return this->_ptr;
    }
    PTR* get() const {
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
