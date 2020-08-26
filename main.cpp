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
        : i(0) 
        , links(this)
        , name(this)
    {}

public:
    rcgc_w_str name;
    rcgc_ptr_vector<CNode> links;
};
class DNode
{
public:
    size_t i;
public:
    DNode()
        : i(0)
        , links(this)
        , name(this)
    {}
    void disposing() {
        this->links.disposing();
    }
public:
    rcgc_w_str name;
    rcgc_d_ptr_vector<DNode> links;
};

class FNode
{
public:
    size_t i;
public:
    FNode()
        : i(0)
        , links(this)
        , name(this)
    {}
    void disposing() {
        this->links.disposing();
    }
    void finalize() {
        //this is the detor
    }
public:
    rcgc_w_str name;
    rcgc_f_ptr_vector<FNode> links;
};

int main() {
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    //srand((unsigned int)time(nullptr));

    rcgc_base::SetAutoCollect(true);
    const size_t MaxNodes = 4;
    rcgc_d_ptr_vector<DNode> dnodes;
    rcgc_f_ptr_vector<FNode> fnodes;

    rcgc_ptr_vector<CNode> cnodes;

    for (size_t i = 0; i < MaxNodes; i++) {
        CNode* n = new CNode();
        n->i = i;
        (*cnodes).push_back(n);
    }

    //full connection
    for (size_t i = 0; i < (*cnodes).size(); i++) {
        for (size_t j = 0; j < (*cnodes).size(); j++) {
            (*cnodes)[i]->links->push_back((*cnodes)[j]);
        }
    }
    rcgc_w_str mystr(L"Hello World");
    rcgc_w_str mystr2 = L"Hello 2";

    return 0;
}