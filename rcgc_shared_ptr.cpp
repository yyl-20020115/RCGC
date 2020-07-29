#include "rcgc_shared_ptr.h"

std::mutex rcgc_base::m;
bool rcgc_base::ac = false;
int rcgc_base::index = 0;
std::unordered_map<void*, int> rcgc_base::_refs;

long long rcgc_base::NextIndex()
{
	return index++;
}

//this relationship is simplified,
//and represented by counting plus one.
void rcgc_base::AddRelation(void* ptr, long long id)
{
	if (ptr != nullptr) {
		std::lock_guard<std::mutex> lock(m);
		auto p = _refs.find(ptr);
		if (p == _refs.end()) {
			_refs.insert(std::make_pair(ptr, 1));
		}
		else {
			p->second++;
		}
	}
}

void rcgc_base::RemoveRelation(void* ptr, long long id)
{
	if (ptr != nullptr) {
		std::lock_guard<std::mutex> lock(m);
		auto p = _refs.find(ptr);
		if (p != _refs.end()) {
			p->second--;
		}
	}
}

bool rcgc_base::SetAutoCollect(bool _ac)
{
	return ac=_ac;
}

bool rcgc_base::GetAutoCollect()
{
	return ac;
}

void rcgc_base::Collect(bool threading, bool join)
{
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
		Collect(_refs);
	}
}

void rcgc_base::CollectThread()
{
	std::unordered_map<void*, int> p_refs;
	{
		std::lock_guard<std::mutex> lock(m);
		p_refs = _refs;
	}
	Collect(p_refs);
}

void rcgc_base::Collect(std::unordered_map<void*, int>& p_refs)
{	
	for (auto p = p_refs.begin(); p != p_refs.end(); ++p) {
		if (p->second == 0) {
			delete p->first;
			p = p_refs.erase(p);
			if (p == p_refs.end())break;
		}
	}
}

