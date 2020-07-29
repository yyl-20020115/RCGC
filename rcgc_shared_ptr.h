#pragma once
#include<mutex>
#include<vector>
#include<unordered_map>

class rcgc_base {
public:
    static bool SetAutoCollect(bool ac);
    static bool GetAutoCollect();
    static void Collect(bool threading = false, bool join = false);
protected:
    static void AddRef(void* ptr);
    static void RelRef(void* ptr);
    static void CollectThread();
    static void Collect(std::vector<void*>& p_wilds);
protected:
    static bool _ac;
    static std::mutex _m;
    static std::unordered_map<void*, size_t> _refs;
    static std::vector<void*> _wilds;
};

template<class PTR>
class rcgc_shared_ptr
    : public rcgc_base
{
public:

    rcgc_shared_ptr(PTR* ptr = nullptr) :_ptr(ptr) {
        AddRef(this->_ptr);
    }
    rcgc_shared_ptr(const rcgc_shared_ptr& src) :_ptr(src._ptr){
        AddRef(this->_ptr);
    }
    virtual ~rcgc_shared_ptr() {
        this->Dispose();
        if (_ac) {
            Collect();
        }
    }
public:
    void Dispose() {
        if (this->_ptr != nullptr) {
            PTR* _ptr = this->_ptr;
            this->_ptr = nullptr;
            _ptr->~PTR();
            RelRef(_ptr);
        }
    }
    rcgc_shared_ptr& operator = (rcgc_shared_ptr<PTR>& src) {
        if (this->_ptr != src._ptr) {
        }
        else if (src._ptr != nullptr) {
            RelRef(this->_ptr);
            AddRef(this->_ptr = src._ptr);
        }
        return *this;
    }

    PTR& operator*() const {
        return *_ptr;
    }
    PTR* operator->() const {
        return _ptr;
    }
protected:
    PTR* _ptr;
};
