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
	};

	/// \brief interface for a property getter that returns a const reference instead of a copy
	template<class T>
	struct IRefGetter
	{
		IRefGetter() = default;
		virtual ~IRefGetter() = default;
		virtual operator const T&() = 0;
	};

	/// \brief interface for a property setter
	template<class T>
	struct ISetter
	{
		ISetter() = default;
		virtual ~ISetter() = default;
		virtual void operator=(T value) = 0;
	};

	namespace detail
	{
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
	}

	/// \brief property implementation with a trivial getter
	template<class T>
	class DefaultGetter : public IGetter<T>, virtual protected detail::DefaultBase<T>
	{
	public:
		DefaultGetter()
			:
		detail::DefaultBase<T>()
		{}
		explicit DefaultGetter(T&& init)
			:
		detail::DefaultBase<T>(std::move(init))
		{}
		operator T() const override final
		{
			return this->value;
		}
	};

	/// \brief property implementation with a trivial setter
	template<class T>
	class DefaultSetter : public ISetter<T>, virtual protected detail::DefaultBase<T>
	{
	public:
		DefaultSetter()
			:
		detail::DefaultBase<T>()
		{}
		explicit DefaultSetter(T&& init)
			:
		detail::DefaultBase<T>(std::move(init))
		{}
		void operator=(T value) override
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
		detail::DefaultBase<T>(std::move(init)),
		DefaultGetter<T>(),
		DefaultSetter<T>()
		{}
		// must be done because overloaded operators will be discarded in inheritance
		virtual void operator=(T value) override
		{
			return DefaultSetter<T>::operator=(std::move(value));
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
		template<class TClass>
		FunctionGetter(const TClass* object, T(TClass::* getter)() const)
			:
		function(std::bind(getter, object))
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
		function(std::move(function))
		{}
		template<class TClass>
		FunctionSetter(TClass* object, void(TClass::* setter)(T))
			:
		function(std::bind(setter, object, std::placeholders::_1))
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
		template<class TClass>
		FunctionGetterSetter(TClass* object, T(TClass::* getter)() const, void(TClass::* setter)(T))
			:
		FunctionGetter<T>(object, getter),
		FunctionSetter<T>(object, setter)
		{}
		// must be done because overloaded operators will be discarded in inheritance
		void operator=(T value) override
		{
			return FunctionSetter<T>::operator=(std::move(value));
		}
	};
}
