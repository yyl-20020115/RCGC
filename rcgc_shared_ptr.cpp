#include "rcgc_shared_ptr.h"

std::mutex rcgc_base::_m;
bool rcgc_base::_ac = false;
std::unordered_map<void*, size_t> rcgc_base::_refs;
std::vector<void*> rcgc_base::_wilds;

//this relationship is simplified,
//and represented by counting plus one.
void* rcgc_base::AddRef(void* ptr)
{
    if (ptr != nullptr) {
        std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p == _refs.end()) {
            _refs.insert(std::make_pair(ptr, 1));
        }
        else {
            p->second++;
        }
        //check if RelRef removed the ptr, undo if found
        if (_wilds.size() > 0 && _wilds.back() == ptr) {
            _wilds.pop_back();
        }
    }
    return ptr;
}

void* rcgc_base::RelRef(void* ptr)
{
    if (ptr != nullptr) {
        std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p != _refs.end()) {
            if (--p->second == 0) {
                _refs.erase(p);
                _wilds.push_back(ptr);
            }
        }
    }
    return ptr;
}

bool rcgc_base::SetAutoCollect(bool ac)
{
    return _ac = ac;
}

bool rcgc_base::GetAutoCollect()
{
    return _ac;
}

void rcgc_base::Collect(bool threading, bool join)
{
    if (_wilds.size() > 0) {
        if (threading) {
            std::thread t(CollectThread);
            if (join) {
                t.join();
            }
            else {
                t.detach();
            }
        }
        else {
            Collect(_wilds);
        }
    }
}

void rcgc_base::CollectThread()
{
    std::vector<void*> p_wilds;
    {
        std::lock_guard<std::mutex> lock(_m);
        p_wilds = _wilds;
        _wilds.clear();
    }
    Collect(p_wilds);
}

void rcgc_base::Collect(std::vector<void*>& p_wilds)
{
    if (p_wilds.size() > 0) {
        for (auto p = p_wilds.begin(); p != p_wilds.end(); ++p) {
            if (*p) {
                //delete p->first;
                //NOTICE: use free instead of delete to avoid double calling destructor
                free(*p);
            }
        }
        p_wilds.clear();
    }
}

