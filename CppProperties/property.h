#pragma once
#include <utility>

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

	template<class T, class TClass>
	class ClassGetter : public IGetter<T>, virtual protected ClassBase<TClass>
	{
	public:
		ClassGetter(const TClass* object, T(TClass::* function)() const)
			:
		ClassBase<TClass>(const_cast<TClass*>(object)),
		function(function)
		{}

		operator T() const override final
		{
			return std::invoke(function, this->object);
		}
	private:
		T(TClass::* function)() const;
	};

	template<class T, class TClass>
	class ClassSetter : public ISetter<T>, virtual protected ClassBase<TClass>
	{
	public:
		ClassSetter(TClass* object, void(TClass::* function)(T))
			:
		ClassBase<TClass>(object),
		function(function)
		{}

		void operator=(T value) override
		{
			std::invoke(function, this->object, std::move(value));
		}
	private:
		void(TClass::* function)(T);
	};

	template<class T, class TClass>
	class ClassGetterSetter final : public ClassGetter<T, TClass>, public ClassSetter<T, TClass>
	{
	public:
		ClassGetterSetter(TClass* object, T(TClass::* getter)() const, void(TClass::* setter)(T))
			:
		ClassBase<TClass>(object),
		ClassGetter<T, TClass>(object, getter),
		ClassSetter<T, TClass>(object, setter)
		{}
		// must be done because it can't find the function for some reason...
		virtual void operator=(T value) override
		{
			return ClassSetter<T, TClass>::operator=(std::move(value));
		}
	};
}
