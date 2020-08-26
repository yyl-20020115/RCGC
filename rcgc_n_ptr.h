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

    rcgc_n_ptr(PTR* ptr = nullptr, void* ctr = nullptr) :_ptr(ptr),_ctr(ctr) {
        AddConnection(this->_ctr, this->_ptr, terminating_function);
    }
    ~rcgc_n_ptr() {
        this->finalize();
    }
public:
    void disposing() {
        
    }
    void finalize() {
        
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
    void* _ctr;
};
