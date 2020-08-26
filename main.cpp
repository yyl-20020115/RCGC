#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <string>
#include "rcgc_ptr.h"
#include "rcgc_d_ptr.h"
#include "rcgc_f_ptr.h"
#include "rcgc_n_ptr.h"
#include "rcgc_c_ptr.h"

class CNode
{
public:
    size_t i;
public:
    CNode() 
        :i(0) 
        ,links(new std::vector<rcgc_ptr<CNode>>(),this)
        ,name(new std::wstring(L"abcd"),this)
    {}

public:
    rcgc_n_ptr<std::wstring> name;
    rcgc_c_ptr<std::vector<rcgc_ptr<CNode>>> links;
};

int main() {
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    //srand((unsigned int)time(nullptr));

    rcgc_base::SetAutoCollect(true);
    const size_t MaxNodes = 1;
    std::vector<rcgc_ptr<CNode>> nodes;

    for (size_t i = 0; i < MaxNodes; i++) {
        CNode* n = new CNode();
        n->i = i;
        nodes.push_back(n);
    }

    //full connection
    for (size_t i = 0; i < nodes.size(); i++) {
        for (size_t j = 0; j < nodes.size(); j++) {
            nodes[i]->links->push_back(nodes[j]);
        }
    }

    return 0;
}