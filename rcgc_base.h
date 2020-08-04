#pragma once
#include<mutex>
#include<thread>
#include<vector>
#include<unordered_map>

typedef void (*terminating_function)(void* p);

class rcgc_base {
public:
    static bool SetAutoCollect(bool ac);
    static bool GetAutoCollect();
    static void Collect(bool threading = false, bool join = false);
protected:
    static void* AddRef(void* ptr, terminating_function tf = nullptr);
    static void* RelRef(void* ptr);
    static void CollectThread();
    static void Collect(std::vector<std::pair<void*, terminating_function>>& p_wilds);
protected:
    static bool _cl;
    static bool _ac;
    static std::mutex _m;
    static std::unordered_map<void*, std::pair<size_t,terminating_function>> _refs;
    static std::vector<std::pair<void*, terminating_function>> _wilds;
};
