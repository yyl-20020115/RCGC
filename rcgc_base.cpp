#include "rcgc_base.h"

std::mutex rcgc_base::_m;
int rcgc_base::_dp = 0;
int rcgc_base::_dm = DefaultMaxDepth;

bool rcgc_base::_ac = false;
bool rcgc_base::_cl = false;
std::unordered_map<void*, std::pair<size_t, terminating_function>> rcgc_base::_refs;
std::vector<std::pair<void*, terminating_function>> rcgc_base::_wilds;
std::vector<void*> rcgc_base::_breaks;

//this relationship is simplified,
//and represented by counting plus one.
void* rcgc_base::AddRef(void* ptr, terminating_function tf)
{
    if (ptr != nullptr) {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p == _refs.end()) {
            _refs.insert(std::make_pair(ptr, std::make_pair(1,tf)));
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
                ptr = nullptr;
            }
        }
    }
    return ptr;
}

size_t rcgc_base::GetCount(void* ptr)
{
    size_t c = 0;
    if (ptr != nullptr) {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        auto p = _refs.find(ptr);
        if (p != _refs.end()) {
            c = p->second.first;
        }
    }
    return c;
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
    if (!_cl) {
        _cl = true;
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
        _cl = false;
    }

}

void rcgc_base::CollectThread()
{
    std::vector<std::pair<void*, terminating_function>> p_wilds;
    {
        //NOTICE: lock is disabled for now
        //std::lock_guard<std::mutex> lock(_m);
        p_wilds = _wilds;
        _wilds.clear();
    }
    Collect(p_wilds);
}

void rcgc_base::Collect(std::vector<std::pair<void*, terminating_function>>& p_wilds)
{
    if (p_wilds.size() > 0) {
        std::vector<std::pair<void*, terminating_function>> lp(p_wilds);
        p_wilds.clear();
        for (auto p = lp.begin(); p != lp.end(); ++p) {
            if (p->first) {
                if (p->second != nullptr) {
                    p->second(p->first);
                }
                else {
                    free(p->first);
                }
            }
        }
    }
}

