#pragma once
#include "rcgc_d_ptr.h"
#include "rcgc_f_ptr.h"

//Container version of rcgc_ptr (wraps containers)
template<class PTR>
class rcgc_c_ptr
    : public rcgc_base
{
public:
    static void terminating_function(void* ptr) {
        delete reinterpret_cast<PTR*>(ptr);
    }
public:
    rcgc_c_ptr(PTR* ptr = nullptr, void* ctr = nullptr) :_ptr(ptr), _ctr(ctr) {
        AddConnection(this->_ctr, this->_ptr, terminating_function);
    }
    ~rcgc_c_ptr() {
        this->disposing();
    }

public:

    void disposing() {
        PTR* p = this->_ptr;
        if (p != nullptr) {
            for (auto& q : *p) {
                q.disposing();
            }
        }
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
    void* get_ctr() const {
        return this->_ctr;
    }

protected:
    PTR* _ptr;
    void* _ctr;
};

template<class PTR>
class rcgc_vector : public rcgc_c_ptr<std::vector<PTR>> {
public:
    rcgc_vector(void* ctr = nullptr) : rcgc_c_ptr<std::vector<PTR>>(new std::vector<PTR>(),ctr) {
    }
};
template<class PTR>
class rcgc_ptr_vector : public rcgc_vector<rcgc_ptr<PTR>> {
public:
    rcgc_ptr_vector(void* ctr = nullptr) 
        : rcgc_vector<rcgc_ptr<PTR>>(ctr) {
    }
};

template<class PTR>
class rcgc_d_ptr_vector : public rcgc_vector<rcgc_d_ptr<PTR>> {
public:
    rcgc_d_ptr_vector(void* ctr = nullptr)
        : rcgc_vector<rcgc_d_ptr<PTR>>(ctr) {
    }
};
template<class PTR>
class rcgc_f_ptr_vector : public rcgc_vector<rcgc_f_ptr<PTR>> {
public:
    rcgc_f_ptr_vector(void* ctr = nullptr)
        : rcgc_vector<rcgc_f_ptr<PTR>>(ctr) {
    }
};
