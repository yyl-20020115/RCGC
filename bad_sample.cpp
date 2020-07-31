#include<iostream>
#include<map>
using namespace std;
namespace bad_sample {
	class mshared_base {
	public:
		static map<void*, int> _map;		//静态数据成员需要在类外进行初始化
	};
	template<typename T>
	class mshared_ptr : public mshared_base
	{
	public:
		mshared_ptr(T* ptr = NULL);		//构造方法
		~mshared_ptr();		//析构方法
		mshared_ptr(mshared_ptr<T>& src);		//拷贝构造
		mshared_ptr& operator = (mshared_ptr<T>& src);		//赋值运算符重载
		T& operator*();		//解引用运算符重载
		T* operator->();	//成员运算符重载
	private:
		T* _ptr;
		//错误：不应根据T*建立不同的_map，否则两个不同类的对象不会在同一个
		//map中计数。
		//static map<T*, int> _map;		//静态数据成员需要在类外进行初始化
	};

	map<void*, int> mshared_base::_map;

	template<typename T>
	mshared_ptr<T>::mshared_ptr(T* ptr)		//构造方法
	{
		cout << "mshared_ptr的 CONSTRUCTOR 方法正被调用！:" << ptr << endl;
		_ptr = ptr;
		_map.insert(make_pair(_ptr, 1));
	}

	template<typename T>
	mshared_ptr<T>::~mshared_ptr()		//析构方法
	{
		//错误原因：智能指针释放的时候总是应该释放其指向的对象
		//若不能如此，也应当调用其指向的对象的析构函数
		//但若真的调用，则会产生循环引用的释放问题。
		//所以在调用的时候，不能释放指针指向的内存，而要通过有控制的
		//调用来仅仅减少引用计数。

		cout << "mshared_ptr的 DESTROCTOR 方法正被调用！:" << this->_ptr << endl;
		if (--_map[_ptr] <= 0 && NULL != _ptr)
		{
			delete _ptr;
			_ptr = NULL;
			_map.erase(_ptr);
		}
	}

	template<typename T>
	mshared_ptr<T>::mshared_ptr(mshared_ptr<T>& src)	//拷贝构造
	{
		cout << "mshared_ptr的 CONSTROCTOR-2 方法正被调用！:" << src._ptr << endl;

		_ptr = src._ptr;
		_map[_ptr]++;
	}

	template<typename T>
	mshared_ptr<T>& mshared_ptr<T>::operator=(mshared_ptr<T>& src)		//赋值运算符重载
	{
		cout << "mshared_ptr的 ASSIGN 方法正被调用！:" << this->_ptr << " => " << src._ptr << endl;
		if (_ptr == src._ptr)
		{
			return *this;
		}

		if (--_map[_ptr] <= 0 && NULL != _ptr)
		{
			delete _ptr;
			_ptr = NULL;
			_map.erase(_ptr);
		}

		_ptr = src._ptr;
		_map[_ptr]++;
		return *this;
	}

	template<typename T>
	T& mshared_ptr<T>::operator*()		//解引用运算符重载
	{
		cout << "mshared_ptr的 & 方法正被调用！:" << this->_ptr << endl;

		return *_ptr;
	}

	template<typename T>
	T* mshared_ptr<T>::operator->()		//成员运算符重载
	{
		cout << "mshared_ptr的 -> 方法正被调用！:" << this->_ptr << endl;

		return _ptr;
	}

	class B;    //同文件，从上至下编译，故而需要告诉类A——类B确实存在
	class A
	{
	public:
		virtual ~A() {
			cout << "A 的 DESTROCTOR 方法正被调用！:" << this << endl;

		}
	public:

		mshared_ptr<B>_ptr_B;
	};
	class B
	{
	public:
		virtual ~B() {
			cout << "B 的 DESTROCTOR 方法正被调用！:" << this << endl;
		}
	public:
		mshared_ptr<A>_ptr_A;
	};

	int bad_main()
	{
		mshared_ptr<A>ptr_A(new A);
		mshared_ptr<B>ptr_B(new B);
		ptr_A->_ptr_B = ptr_B;
		ptr_B->_ptr_A = ptr_A;
		return 0;
	}

}