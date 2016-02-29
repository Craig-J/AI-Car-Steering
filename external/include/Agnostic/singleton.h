#ifndef _AGNOSTIC_SINGLETON_H_
#define _AGNOSTIC_SINGLETON_H_

namespace agn
{
	// LocalStaticSingleton
	// This singleton style uses an instance stored as a static local variable in the GetInstance function.
	// + The singleton instance is stack allocated.
	// + Thread safe initialization.
	// - Must be careful with destructor sequence. (e.g. when using nested singleton)
	template<typename _Type>
	class LocalStaticSingleton
	{
	private:

		// No copying
		LocalStaticSingleton(const LocalStaticSingleton&) = delete;

		// No assignment
		LocalStaticSingleton & operator=(const LocalStaticSingleton&) = delete;

		// Private construction/destruction only
		LocalStaticSingleton() {};
		~LocalStaticSingleton() {};

	public:

		// GetInstance
		// Returns the singleton, with lazy initialization.
		static _Type& GetInstance()
		{
			return(static _Type instance);
		}
	};

	// Singleton
	// This is the standard singleton with two use cases.
	//
	// The first use case is to typdef a singleton class from another class.
	// Example:
	//		typedef Singleton<ClassType> ClassName;
	//
	// The second is to use CRTP (Curiously Recurring Template Pattern) to create a singleton child class.
	// The child class should have a private constructor and make Singleton a friend.
	// This is to stop the derived class from being constructed externally.
	// Example:
	//		class Derived : public Singleton<DerivedClass>
	//		{
	//		private:
	//			Derived() {};
	//			friend class Singleton<DerivedClass>;
	//		}
	template<typename _Type>
	class Singleton
	{
	private:

		// The one and only static instance
		static _Type* instance_;

		// No copying
		Singleton(const Singleton&) = delete;

		// No assignment
		Singleton & operator=(const Singleton&) = delete;

	protected:

		// Protected to allow derived classes to construct using CRTP
		Singleton() {};
		~Singleton() {};

	public:

		// GetInstance
		// Returns the singleton, with lazy initialization.
		static _Type& GetInstance()
		{
			return(instance_ ? *instance_ : *(instance_ = new _Type));
		}
	};

	// Initialises static Type* instance_ member to nullptr.
	template <typename _Type>
	_Type* Singleton<_Type>::instance_ = nullptr;
}

#endif // _AGNOSTIC_SINGLETON_H_