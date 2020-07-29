#pragma once
#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<mutex>

class rcgc_base {
public:
    static bool SetAutoCollect(bool ac);
    static bool GetAutoCollect();
    static void Collect(bool threading = false, bool join = false);
protected:
    static size_t NextIndex();
    static void AddRelation(void* ptr, size_t id);
    static void RemoveRelation(void* ptr, size_t id);
    static void CollectThread();
    static void Collect(std::vector<void*>& p_wilds);
protected:
    static bool ac;
    static size_t index;
    static std::mutex m;
    static std::unordered_map<void*, size_t> _refs;
    static std::vector<void*> _wilds;
};

template<class PTR>
class rcgc_shared_ptr
    : public rcgc_base
{
public:

    rcgc_shared_ptr(PTR* ptr = nullptr) :_ptr(ptr), _id(NextIndex()) {
        AddRelation(this->_ptr, this->_id);
    }
    rcgc_shared_ptr(const rcgc_shared_ptr& src) :_ptr(src._ptr), _id(NextIndex()) {
        AddRelation(this->_ptr, this->_id);
    }
    virtual ~rcgc_shared_ptr() {
        this->Dispose();
        if (ac) {
            Collect();
        }
    }
public:
    void Dispose() {
        if (this->_ptr != nullptr) {
            PTR* _ptr = this->_ptr;
            this->_ptr = nullptr;

            _ptr->~PTR();

            RemoveRelation(_ptr, _id);
        }
    }
    rcgc_shared_ptr& operator = (rcgc_shared_ptr<PTR>& src) {
        if (this->_ptr == src._ptr)
            return *this;
        else if (src._ptr != nullptr) {
            RemoveRelation(this->_ptr, this->_id);
            AddRelation(this->_ptr = src._ptr, this->_id);
        }
        return *this;
    }

    PTR& operator*() {
        return *_ptr;
    }
    PTR* operator->() {
        return _ptr;
    }
    long long GetId() {
        return this->_id;
    }
protected:
    const size_t _id;
    PTR* _ptr;
};
