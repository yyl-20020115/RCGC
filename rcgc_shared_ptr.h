#pragma once
#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<mutex>

class rcgc_base {
public:
	static bool SetAutoCollect(bool ac);
	static bool GetAutoCollect();
	static void Collect(bool threading =false, bool join = false);
protected:
	static long long NextIndex();
	static void AddRelation(void* ptr, long long id);
	static void RemoveRelation(void* ptr, long long id);
	static void CollectThread();
	static void Collect(std::unordered_map<void*, int>& p_refs);
protected:
	static bool ac;
	static int index;
	static std::mutex m;
	static std::unordered_map<void*, int> _refs;
};

template<class PTR>
class rcgc_shared_ptr 
	: public rcgc_base
{
public:

	rcgc_shared_ptr(PTR* ptr = nullptr) :_ptr(ptr), _id(NextIndex()) {
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
	const long long _id;
	PTR* _ptr;
};
