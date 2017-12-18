#pragma once

#include <cassert>
#include <memory>


/*!
	class ICallback

	\brief The class represents an interface of a callback 
*/
template<typename TResult, typename... TArgs>
class ICallback
{
	public:
		ICallback() = default;
		virtual ~ICallback() = default;
		
		/*!
			\brief The method causes call of a specified callback
			\param [in] args The arguments that will be send to the callback

			\return Some value of TResult type
		*/
		virtual TResult Invoke(TArgs... args) const = 0;

		/*!
			\brief The method implements comparator's functionality and allows to
			compare two callbacks by their content

			\return The method returns true if both callbacks' objects have the same type
			and point to the same functions
		*/
		virtual bool EqualsTo(const ICallback<TResult, TArgs...>* pCallback) const = 0;
};


/*!
	class CStaticCallback

	\brief The class implements ICallback interface
*/
template<typename TResult, typename... TArgs>
class CStaticCallback : public ICallback<TResult, TArgs...>
{
	protected:
		typedef TResult(*TCallbackPtr)(TArgs...);
	public:
		CStaticCallback(TCallbackPtr callee)
		{
			mpCalleeFunc = callee;

			assert(mpCalleeFunc != nullptr);
		}

		virtual ~CStaticCallback()
		{
			mpCalleeFunc = nullptr;
		}

		/*!
			\brief The method causes call of a specified callback
			\param [in] args The arguments that will be send to the callback

			\return Some value of TResult type
		*/
		TResult Invoke(TArgs... args) const override
		{
			if (mpCalleeFunc != nullptr)
			{
				(*mpCalleeFunc)(args...);
			}
		}

		/*!
			\brief The method implements comparator's functionality and allows to
			compare two callbacks by their content

			\return The method returns true if both callbacks' objects have the same type
			and point to the same functions
		*/
		bool EqualsTo(const ICallback<TResult, TArgs...>* pCallback) const override
		{
			const CStaticCallback<TResult, TArgs...>* pStaticCallback = dynamic_cast<const CStaticCallback<TResult, TArgs...>*>(pCallback);

			if (!pStaticCallback)
			{
				return false;
			}

			return pStaticCallback->mpCalleeFunc == mpCalleeFunc;
		}

		static std::unique_ptr<ICallback<TResult, TArgs...>> CreateCallback(TCallbackPtr callee)
		{
			return std::make_unique<CStaticCallback>(callee);
		}
	protected:
		TCallbackPtr mpCalleeFunc;
};


/*!
	class CMethodCallback

	\brief The class implements ICallback
*/
template<typename TClass, typename TResult, typename... TArgs>
class CMethodCallback : public ICallback<TResult, TArgs...>
{
	protected:
		typedef TResult(TClass::*TCallbackPtr)(TArgs...);
	public:
		CMethodCallback(TClass* pObject, TCallbackPtr callee)
		{
			mpCalleeFunc = callee;

			mpCallerObject = pObject;

			assert(mpCalleeFunc != nullptr);
			assert(mpCallerObject != nullptr);
		}

		virtual ~CMethodCallback()
		{
			mpCalleeFunc = nullptr;
		}

		/*!
			\brief The method causes call of a specified callback
			\param [in] args The arguments that will be send to the callback

			\return Some value of TResult type
		*/
		TResult Invoke(TArgs... args) const override
		{
			if (mpCalleeFunc != nullptr &&
				mpCallerObject != nullptr)
			{
				(mpCallerObject->*mpCalleeFunc)(args...);
			}
		}

		/*!
			\brief The method implements comparator's functionality and allows to
			compare two callbacks by their content

			\return The method returns true if both callbacks' objects have the same type
			and point to the same functions
		*/
		bool EqualsTo(const ICallback<TResult, TArgs...>* pCallback) const override
		{
			const CMethodCallback<TClass, TResult, TArgs...>* pMethodCallback = dynamic_cast<const CMethodCallback<TClass, TResult, TArgs...>*>(pCallback);

			if (!pMethodCallback)
			{
				return false;
			}

			return pMethodCallback->mpCalleeFunc == mpCalleeFunc &&
				   pMethodCallback->mpCallerObject == mpCallerObject;
		}

		static std::unique_ptr<ICallback<TResult, TArgs...>> CreateCallback(TClass* pCallerObject, TCallbackPtr callee)
		{
			return std::make_unique<CMethodCallback>(pCallerObject, callee);
		}
	protected:
		TCallbackPtr mpCalleeFunc;
		TClass*      mpCallerObject;
};