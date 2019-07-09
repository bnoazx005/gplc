#pragma once


#include "Callback.h"
#include <vector>
#include <memory>
#include <tuple>


/*!
	class CDelegate

	\brief The class represents a multi-cast delegate object.
*/
template <typename TResult, typename... TArgs>
class CDelegate
{
	public:
		CDelegate()
		{
		}

		~CDelegate()
		{
			mpCallbacks.clear();
		}

		/*!
			\brief The method causes multicasting of specified arguments among the subscribers of
			the delegate
			\param [in] args The arguments that will be send to subscribers
		*/
		void Invoke(TArgs... args) const
		{
			for (auto iter = mpCallbacks.cbegin(); iter != mpCallbacks.cend(); ++iter)
			{
				(*iter)->Invoke(args...);
			}
		}

		/*!
			\brief The operator do the same things as Invoke. So it just an alias for latter.
			\param [in] args The arguments that will be send to subscribers
		*/
		void operator() (TArgs... args) const
		{
			Invoke(args...);
		}

		/*!
			\brief The operator provides a subscription's mechanism for static callbacks
			\param[in] pCalleeFunc A callback (a pointer to a function)
		*/
		void operator+= (TResult(*pCalleeFunc)(TArgs...))
		{
			_addCallback(pCalleeFunc);
		}

		/*!
			\brief The operator provides a subscription's mechanism for method callbacks
			\param[in] methodCallback A tuple that consists of a pointer to an object and a pointer to its method
		*/
		template <typename TClass>
		void operator+= (std::tuple<TClass*, TResult (TClass::*)(TArgs...)> methodCallback)
		{
			_addCallback(std::get<0>(methodCallback), std::get<1>(methodCallback));
		}

		/*!
			\brief The operator provides a mechanism of unsubsription of static callbacks
			\param[in] pCalleeFunc A callback (a pointer to a function)
		*/
		void operator-= (TResult(*pCalleeFunc)(TArgs...))
		{
			_removeCallback(std::make_unique<const CStaticCallback<TResult, TArgs...>>(pCalleeFunc).get());
		}

		/*!
			\brief The operator provides a mechanism of unsubsription of classes' method callbacks
			\param[in] methodCallback A tuple that consists of a pointer to an object and a pointer to its method
		*/
		template <typename TClass>
		void operator-= (std::tuple<TClass*, TResult(TClass::*)(TArgs...)> methodCallback)
		{
			_removeCallback(std::make_unique<const CMethodCallback<TClass, TResult, TArgs...>>(
									std::get<0>(methodCallback), std::get<1>(methodCallback)).get());
		}
	protected:
		void _addCallback(TResult(*pCalleeFunc)(TArgs...))
		{
			_addCallback(CStaticCallback<TResult, TArgs...>::CreateCallback(pCalleeFunc));
		}

		template<typename TClass>
		void _addCallback(TClass* pObject, TResult(TClass::*pCalleeMethod)(TArgs...))
		{
			_addCallback(CMethodCallback<TClass, TResult, TArgs...>::CreateCallback(pObject, pCalleeMethod));
		}

		void _addCallback(std::unique_ptr<ICallback<TResult, TArgs...>> pCallback)
		{
			assert(pCallback != nullptr);

			if (pCallback == nullptr)
			{
				return;
			}

			mpCallbacks.push_back(std::move(pCallback));
		}

		void _removeCallback(const ICallback<TResult, TArgs...>* pCallback)
		{
			assert(pCallback != nullptr);

			if (!pCallback)
			{
				return;
			}

			auto iter = mpCallbacks.begin();

			while (iter != mpCallbacks.end())
			{
				if (pCallback->EqualsTo((*iter).get()))
				{
					iter = mpCallbacks.erase(iter);

					continue;
				}

				++iter;
			}
		}
	protected:
		std::vector<std::unique_ptr<ICallback<TResult, TArgs...>>> mpCallbacks;
};


/*!
	\brief The function creates a tuple that contains all needed information to subscribe on some delegate

	\param [in] pObject A pointer to an object of TClass type
	\param [int] pCalleeFunc A pointer to method of class TClass with the signature TResult m(TArgs...)

	\return A tuple that consists of a pointer to an object and a pointer to its method
*/
template<typename TClass, typename TResult, typename... TArgs>
inline std::tuple<TClass*, TResult(TClass::*)(TArgs...)> MakeMethodDelegate(TClass* pObject, TResult(TClass::*pCalleeFunc)(TArgs...))
{
	return std::make_tuple(pObject, pCalleeFunc);
}