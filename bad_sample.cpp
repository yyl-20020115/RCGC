#include<iostream>
#include<map>
using namespace std;
namespace bad_sample {
	class mshared_base {
	public:
		static map<void*, int> _map;		//��̬���ݳ�Ա��Ҫ��������г�ʼ��
	};
	template<typename T>
	class mshared_ptr : public mshared_base
	{
	public:
		mshared_ptr(T* ptr = NULL);		//���췽��
		~mshared_ptr();		//��������
		mshared_ptr(mshared_ptr<T>& src);		//��������
		mshared_ptr& operator = (mshared_ptr<T>& src);		//��ֵ���������
		T& operator*();		//���������������
		T* operator->();	//��Ա���������
	private:
		T* _ptr;
		//���󣺲�Ӧ����T*������ͬ��_map������������ͬ��Ķ��󲻻���ͬһ��
		//map�м�����
		//static map<T*, int> _map;		//��̬���ݳ�Ա��Ҫ��������г�ʼ��
	};

	map<void*, int> mshared_base::_map;

	template<typename T>
	mshared_ptr<T>::mshared_ptr(T* ptr)		//���췽��
	{
		cout << "mshared_ptr�� CONSTRUCTOR �����������ã�:" << ptr << endl;
		_ptr = ptr;
		_map.insert(make_pair(_ptr, 1));
	}

	template<typename T>
	mshared_ptr<T>::~mshared_ptr()		//��������
	{
		//����ԭ������ָ���ͷŵ�ʱ������Ӧ���ͷ���ָ��Ķ���
		//��������ˣ�ҲӦ��������ָ��Ķ������������
		//������ĵ��ã�������ѭ�����õ��ͷ����⡣
		//�����ڵ��õ�ʱ�򣬲����ͷ�ָ��ָ����ڴ棬��Ҫͨ���п��Ƶ�
		//�����������������ü�����

		cout << "mshared_ptr�� DESTROCTOR �����������ã�:" << this->_ptr << endl;
		if (--_map[_ptr] <= 0 && NULL != _ptr)
		{
			delete _ptr;
			_ptr = NULL;
			_map.erase(_ptr);
		}
	}

	template<typename T>
	mshared_ptr<T>::mshared_ptr(mshared_ptr<T>& src)	//��������
	{
		cout << "mshared_ptr�� CONSTROCTOR-2 �����������ã�:" << src._ptr << endl;

		_ptr = src._ptr;
		_map[_ptr]++;
	}

	template<typename T>
	mshared_ptr<T>& mshared_ptr<T>::operator=(mshared_ptr<T>& src)		//��ֵ���������
	{
		cout << "mshared_ptr�� ASSIGN �����������ã�:" << this->_ptr << " => " << src._ptr << endl;
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
	T& mshared_ptr<T>::operator*()		//���������������
	{
		cout << "mshared_ptr�� & �����������ã�:" << this->_ptr << endl;

		return *_ptr;
	}

	template<typename T>
	T* mshared_ptr<T>::operator->()		//��Ա���������
	{
		cout << "mshared_ptr�� -> �����������ã�:" << this->_ptr << endl;

		return _ptr;
	}

	class B;    //ͬ�ļ����������±��룬�ʶ���Ҫ������A������Bȷʵ����
	class A
	{
	public:
		virtual ~A() {
			cout << "A �� DESTROCTOR �����������ã�:" << this << endl;

		}
	public:

		mshared_ptr<B>_ptr_B;
	};
	class B
	{
	public:
		virtual ~B() {
			cout << "B �� DESTROCTOR �����������ã�:" << this << endl;
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