#include "rcgc_shared_ptr.h"

std::mutex rcgc_base::m;
bool rcgc_base::ac = false;
int rcgc_base::index = 0;
std::unordered_map<void*, int> rcgc_base::_refs;
std::vector<void*> rcgc_base::_wilds;

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
			if (--p->second == 0) {
				_refs.erase(p);
				_wilds.push_back(ptr);
			}
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
		Collect(_wilds);
	}
}

void rcgc_base::CollectThread()
{
	std::vector<void*> p_refs;
	{
		std::lock_guard<std::mutex> lock(m);
		p_refs = _wilds;
		_wilds.clear();
	}
	Collect(p_refs);
}

void rcgc_base::Collect(std::vector<void*>& p_wilds)
{	
	for (auto p = p_wilds.begin(); p != p_wilds.end(); ++p) {
		if (*p) {
			//delete p->first;
			//NOTICE: use free instead of delete to avoid double calling destructor
			free(*p);
			p = p_wilds.erase(p);
			if (p == p_wilds.end())break;
		}
	}
}

