#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include "rcgc_shared_ptr.h"

class B;
class A
{
public:
    A() : _ptr_outB1()
        , _ptr_outB2() {}

public:
    rcgc_shared_ptr<A> _ptr_outA1;
    rcgc_shared_ptr<B> _ptr_outB1;
    rcgc_shared_ptr<B> _ptr_outB2;
};
class B
{
public:
    B() :_ptr_outA() {}

public:
    rcgc_shared_ptr<A> _ptr_outA;
};


struct C
{
    unsigned int x = 0u;

    ~C() { x = 0xBAADF00D; }
};

//struct E;
//
//struct D
//{
//    rcgc_shared_ptr< E > m_b;
//    std::string m_x;
//
//    ~D() { std::cout << m_b->m_x << std::endl; }
//};
//
//struct E
//{
//    rcgc_shared_ptr< D > m_a;
//    std::string m_x;
//
//    ~E() { std::cout << m_a->m_x << std::endl; }
//};
//




//Here is a simple method to solve circulating reference problem of shared_ptr without using weak_ptr.
int main()
{
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    rcgc_base::SetAutoCollect(true);

    rcgc_shared_ptr<A> ptr_A(new A);
    rcgc_shared_ptr<B> ptr_B(new B);

    ptr_A->_ptr_outA1 = ptr_A;
    ptr_A->_ptr_outB1 = ptr_B;
    ptr_A->_ptr_outB2 = ptr_B;
    ptr_B->_ptr_outA = ptr_A;

    /// <summary>
    /// case 1
    /// </summary>
    /// <returns></returns>
    rcgc_shared_ptr< C > p1;
    {
        rcgc_shared_ptr< C > p2(new C());
        p1 = p2;
        std::cout <<std::hex<< p1->x << std::endl;
    }
    std::cout << std::hex << p1->x << std::endl; // (1)

    /// <summary>
    /// case 2
    /// </summary>
    /// <returns></returns>
    //rcgc_shared_ptr< D > p(new D());
    //rcgc_shared_ptr< E > p2(new E());
    //
    //p->m_b->m_a = p;


}
