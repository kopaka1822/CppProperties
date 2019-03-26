#pragma once
#include <utility>
#include <functional>

namespace prop
{
	/// \brief interface for a property getter
	template<class T>
	struct IGetter
	{
		IGetter() = default;
		virtual ~IGetter() = default;
		virtual operator T() const = 0;
		// should not be copied
		IGetter(const IGetter&) = delete;
		IGetter& operator=(const IGetter&) = delete;
	};

	/// \brief interface for a property getter that returns a const reference instead of a copy
	template<class T>
	struct IRefGetter
	{
		IRefGetter() = default;
		virtual ~IRefGetter() = default;
		virtual operator const T&() = 0;
		// should not be copied
		IRefGetter(const IRefGetter&) = delete;
		IRefGetter& operator=(const IRefGetter&) = delete;
	};

	/// \brief interface for a property setter
	template<class T>
	struct ISetter
	{
		ISetter() = default;
		virtual ~ISetter() = default;
		virtual void operator=(T value) = 0;
		// should not be copied
		ISetter(const ISetter&) = delete;
		ISetter& operator=(const ISetter&) = delete;
	};

	template<class T>
	class DefaultBase
	{
	protected:
		DefaultBase() = default;
		explicit DefaultBase(T init)
			:
		value(std::move(init))
		{}

		T value;
	};

	/// \brief property implementation with a trivial getter
	template<class T>
	class DefaultGetter : public IGetter<T>, virtual protected DefaultBase<T>
	{
	public:
		DefaultGetter() = default;
		explicit DefaultGetter(T&& init)
			:
		DefaultBase<T>(std::move(init))
		{}
		virtual operator T() const override final
		{
			return this->value;
		}
	};

	/// \brief property implementation with a trivial setter
	template<class T>
	class DefaultSetter : public ISetter<T>, virtual protected DefaultBase<T>
	{
	public:
		DefaultSetter() = default;
		explicit DefaultSetter(T&& init)
			:
			DefaultBase<T>(std::move(init))
		{}
		virtual void operator=(T value) override
		{
			this->value = std::move(value);
		}
	};

	/// \brief property implementation with trivial getter/setter
	template<class T>
	class DefaultGetterSetter final : public DefaultGetter<T>, public DefaultSetter<T>
	{
	public:
		DefaultGetterSetter() = default;
		explicit DefaultGetterSetter(T&& init)
			:
		DefaultBase<T>(std::move(init)),
		DefaultGetter<T>(),
		DefaultSetter<T>()
		{}
		// must be done because it can't find the function for some reason...
		virtual void operator=(T value) override
		{
			return DefaultSetter<T>::operator=(std::move(value));
		}
	};

	template<class T>
	class ClassBase
	{
	protected:
		ClassBase(T* object)
			:
		object(object)
		{}
		T* object;
	};

	template<class T, class TClass, T(TClass::* FuncAddr)() const>
	class ClassGetter : public IGetter<T>, virtual protected ClassBase<TClass>
	{
	public:
		ClassGetter(const TClass* object)
			:
		ClassBase<TClass>(const_cast<TClass*>(object))
		{}

		operator T() const override final
		{
			return std::invoke(FuncAddr, this->object);
		}
	};

	template<class T, class TClass, void(TClass::* FuncAddr)(T)>
	class ClassSetter : public ISetter<T>, virtual protected ClassBase<TClass>
	{
	public:
		ClassSetter(TClass* object)
			:
		ClassBase<TClass>(object)
		{}

		void operator=(T value) override
		{
			std::invoke(FuncAddr, this->object, std::move(value));
		}
	};

	template<class T, class TClass, T(TClass::* GetterAddr)() const, void(TClass::* SetterAddr)(T)>
	class ClassGetterSetter final : public ClassGetter<T, TClass, GetterAddr>, public ClassSetter<T, TClass, SetterAddr>
	{
	public:
		ClassGetterSetter(TClass* object)
			:
		ClassBase<TClass>(object),
		ClassGetter<T, TClass, GetterAddr>(object),
		ClassSetter<T, TClass, SetterAddr>(object)
		{}
		// must be done because it can't find the function for some reason...
		virtual void operator=(T value) override
		{
			return ClassSetter<T, TClass, SetterAddr>::operator=(std::move(value));
		}
	};

	template<class T>
	class FunctionGetter : public IGetter<T>
	{
	public:
		FunctionGetter(std::function<T()> function)
			:
		function(std::move(function))
		{}

		operator T() const override
		{
			return function();
		}
	private:
		std::function<T()> function;
	};

	template<class T>
	class FunctionSetter : public ISetter<T>
	{
	public:
		FunctionSetter(std::function<void(T)> function)
			:
		function(function)
		{}

		void operator=(T value) override
		{
			function(std::move(value));
		}
	private:
		std::function<void(T)> function;
	};

	template<class T>
	class FunctionGetterSetter : public FunctionGetter<T>, public FunctionSetter<T>
	{
	public:
		FunctionGetterSetter(std::function<T()> getter, std::function<void(T)> setter)
			:
		FunctionGetter<T>(std::move(getter)),
		FunctionSetter<T>(std::move(setter))
		{}
		// must be done because it can't find the function for some reason...
		virtual void operator=(T value) override
		{
			return FunctionSetter<T>::operator=(std::move(value));
		}
	};
}
