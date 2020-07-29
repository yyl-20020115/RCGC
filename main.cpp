#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include "rcgc_shared_ptr.h"

class B;   
class A
{
public:
	A():  _ptr_outB1()
		, _ptr_outB2() {}

public:
	rcgc_shared_ptr<A> _ptr_outA1;
	rcgc_shared_ptr<B> _ptr_outB1;
	rcgc_shared_ptr<B> _ptr_outB2;
};
class B
{
public:
	B():_ptr_outA(){}

public:
	rcgc_shared_ptr<A> _ptr_outA;
};

//Here is a simple method to solve circulating reference problem of shared_ptr without using weak_ptr.
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
	
	rcgc_base::SetAutoCollect(true);

	rcgc_shared_ptr<A> ptr_A(new A);
	rcgc_shared_ptr<B> ptr_B(new B);

	ptr_A->_ptr_outA1 = ptr_A;
	ptr_A->_ptr_outB1 = ptr_B;
	ptr_A->_ptr_outB2 = ptr_B;
	ptr_B->_ptr_outA = ptr_A;
}
