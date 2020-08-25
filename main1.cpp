#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include "rcgc_ptr.h"
#include "rcgc_d_ptr.h"
#include "rcgc_f_ptr.h"
#include "rcgc_n_ptr.h"

class FNode
{
public:
    size_t i;
public:
    FNode() :i(0),links(new std::vector<rcgc_f_ptr<FNode>>()){}
    void disposing()
    {
        for (auto& link : *this->links)
        {
            link.disposing();
        }
    }
    void finalize() {
        delete this->links;
    }
public:
    std::vector<rcgc_f_ptr<FNode>>* links;
};

int main1() {
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    //srand((unsigned int)time(nullptr));

    rcgc_base::SetAutoCollect(true);
    const size_t MaxNodes = 16;
    std::vector<rcgc_f_ptr<FNode>> nodes;

    for (size_t i = 0; i < MaxNodes; i++) {
        FNode* n = new FNode();
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