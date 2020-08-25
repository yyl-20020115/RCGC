#pragma once
#include<mutex>
#include<thread>
#include<vector>
#include<unordered_map>

typedef void (*terminating_function)(void* p);

class rcgc_base {
public:
    static const int DefaultMaxDepth = 1024;
public:
    static bool SetAutoCollect(bool ac);
    static bool GetAutoCollect();
    static void Collect(bool threading = false, bool join = false);
protected:
    static void* AddRef(void* ptr, terminating_function tf = nullptr);
    static void* RelRef(void* ptr);
    static size_t GetCount(void* ptr);

    static void CollectThread();
    static void Collect(std::vector<std::pair<void*, terminating_function>>& p_wilds);

    template<class RCGCPTR, class PTR>
    static void OnCollecting(RCGCPTR* rcgc);

protected:
    static bool _cl;
    static bool _ac;
    static int _dp;
    static int _dm;
    static std::mutex _m;
    static std::unordered_map<void*, std::pair<size_t,terminating_function>> _refs;
    static std::vector<std::pair<void*, terminating_function>> _wilds;
    static std::vector<void*> _breaks;

};

template<class RCGCPTR,class PTR>
inline void rcgc_base::OnCollecting(RCGCPTR* rcgc)
{
    ++_dp;
    //check the max depth
    if (_dp == _dm) {
        //the recursion is too deep
        //we have to cut the link
        //and save the reference
        //and refree the reference in the end of this round.
        _breaks.push_back(reinterpret_cast<void*>(rcgc->rebind()));
    }
    else {
        rcgc->disposing();
        if (_ac) {
            Collect();
        }
    }
    --_dp;
    if (_dp == 0 && _breaks.size()>0 && _ac) {
        std::vector<void*> n_breaks(_breaks);
        _breaks.clear();
        //try to free the breaks again
        //this still may results in deep recursives
        //but maybe not as easy.
        for (void* ip : n_breaks) {
            RCGCPTR().rebind(reinterpret_cast<PTR*>(ip));
        }
    }
}
