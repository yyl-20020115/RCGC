#pragma once
#include "rcgc_base.h"
//class PTR should have a method: void disposing()
template<class PTR>
class rcgc_d_ptr
    : public rcgc_base
{
public:
    static void delete_function(void* ptr) {
        delete reinterpret_cast<PTR*>(ptr);
    }

public:

    rcgc_d_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr, delete_function);
    }
    rcgc_d_ptr(const rcgc_d_ptr& src) :_ptr(src._ptr) {
        AddRef(this->_ptr, delete_function);
    }
    ~rcgc_d_ptr() {
        this->disposing();
        if (_ac) {
            if (!_cl) {
                _cl = true;
                Collect(true);
                _cl = false;
            }
        }
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
            AddRef(this->_ptr = src._ptr, delete_function);
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
protected:
    PTR* _ptr;
};
