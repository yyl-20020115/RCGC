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
        this->disposing();
        if (!_cl) {
            _cl = true;
            Collect();
            _cl = false;
        }
    }
public:
    void disposing() {
        if (this->_ptr != nullptr) {
            PTR* _ptr = this->_ptr;
            this->_ptr = nullptr;
            _ptr->~PTR();
            RelRef(_ptr);
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
