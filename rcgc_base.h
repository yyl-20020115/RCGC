#pragma once
#include<mutex>
#include<thread>
#include<vector>
#include<unordered_map>

typedef void (*delete_function)(void* p);

class rcgc_base {
public:
    static bool SetAutoCollect(bool ac);
    static bool GetAutoCollect();
    static void Collect(bool d_version, bool threading = false, bool join = false);
protected:
    static void* AddRef(void* ptr, delete_function fd = nullptr);
    static void* RelRef(void* ptr);
    static void CollectThread(bool d_version);
    static void Collect(std::vector<std::pair<void*, delete_function>>& p_wilds, bool d_version = true);
protected:
    static bool _cl;
    static bool _ac;
    static std::mutex _m;
    static std::unordered_map<void*, std::pair<size_t,delete_function>> _refs;
    static std::vector<std::pair<void*, delete_function>> _wilds;
};
