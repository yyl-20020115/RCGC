#include "rcgc_ptr.h"

std::mutex rcgc_base::_m;
bool rcgc_base::_ac = false;
bool rcgc_base::_cl = false;
std::unordered_map<void*, std::pair<size_t, delete_function>> rcgc_base::_refs;
std::vector<std::pair<void*, delete_function>> rcgc_base::_wilds;

//this relationship is simplified,
//and represented by counting plus one.
void* rcgc_base::AddRef(void* ptr, delete_function fd)
{
    if (ptr != nullptr) {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p == _refs.end()) {
            _refs.insert(std::make_pair(ptr, std::make_pair(1,fd)));
        }
        else {
            p->second.first++;
        }
    }
    return ptr;
}

void* rcgc_base::RelRef(void* ptr)
{
    if (ptr != nullptr) {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p != _refs.end()) {
            if (--p->second.first == 0) {
                _wilds.push_back(std::make_pair(ptr,p->second.second));
                _refs.erase(p);
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
#if 0
        //NOTICE:don't use threading if lock is disabled
        //NOTICE:threading function not tested
        if (threading) {
            std::thread t(CollectThread);
            if (join) {
                t.join();
            }
            else {
                t.detach();
            }
        }
        else 
#endif
        {
            Collect(_wilds);
        }
    }
}

void rcgc_base::CollectThread()
{
    std::vector<std::pair<void*, delete_function>> p_wilds;
    {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        p_wilds = _wilds;
        _wilds.clear();
    }
    Collect(p_wilds);
}

void rcgc_base::Collect(std::vector<std::pair<void*, delete_function>>& p_wilds)
{
    if (p_wilds.size() > 0) {
        for (auto p = p_wilds.begin(); p != p_wilds.end(); ++p) {
            if (p->first) {
                if (p->second != nullptr) {
                    p->second(p->first);
                }
                else {
                    free(p->first);
                }
            }
        }
        p_wilds.clear();
    }
}

