#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include "rcgc_ptr.h"
#include "rcgc_d_ptr.h"


class B;
class A
{
public:
    A() : _ptr_outB1()
        , _ptr_outB2() {}
    ~A() {
        std::cout << "dtor for object of A:" << std::hex << this << std::endl;
    }
public:
    rcgc_ptr<A> _ptr_outA1;
    rcgc_ptr<B> _ptr_outB1;
    rcgc_ptr<B> _ptr_outB2;
};
class B
{
public:
    B() :_ptr_outA() {}
    ~B() {
        std::cout << "dtor for object of B:" << std::hex << this << std::endl;
    }
public:
    rcgc_ptr<A> _ptr_outA;
};


class D;
class C
{
public:
    C() : _ptr_outD1()
        , _ptr_outD2(){}
    ~C() {
        std::cout << "dtor for object of C:" << std::hex << this << std::endl;
    }
public:
    void disposing();

public:
    rcgc_d_ptr<C> _ptr_outC1;
    rcgc_d_ptr<D> _ptr_outD1;
    rcgc_d_ptr<D> _ptr_outD2;
};

class D
{
public:
    D() :_ptr_outC(){}
    ~D() {
        std::cout << "dtor for object of D:" << std::hex << this << std::endl;
    }
public:
    void disposing() {
        //Should NOT be this->_ptr_outC->disposing() 
        //which will lead to this function itself (direct/indirect-recursion).
        //NOTICE:Don't call self directly or indirectly!
        this->_ptr_outC.disposing();
    }
public:
    rcgc_d_ptr<C> _ptr_outC;
};

void C::disposing()
{
    //NOTICE:Don't call self directly or indirectly as above!
    this->_ptr_outC1.disposing();
    this->_ptr_outD1.disposing();
    this->_ptr_outD2.disposing();
}

int main()
{
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    rcgc_base::SetAutoCollect(true);
    //dtor version
    rcgc_ptr<A> ptr_A(new A);
    rcgc_ptr<B> ptr_B(new B);

    ptr_A->_ptr_outA1 = ptr_A;
    ptr_A->_ptr_outB1 = ptr_B;
    ptr_A->_ptr_outB2 = ptr_B;
    ptr_B->_ptr_outA = ptr_A;

    //disposing version
    rcgc_d_ptr<C> ptr_C(new C);
    rcgc_d_ptr<D> ptr_D(new D);

    ptr_C->_ptr_outC1 = ptr_C;
    ptr_C->_ptr_outD1 = ptr_D;
    ptr_C->_ptr_outD2 = ptr_D;
    ptr_D->_ptr_outC = ptr_C;

}
