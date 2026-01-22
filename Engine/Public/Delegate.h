#pragma once
#include "Base.h"

NS_BEGIN(Engine)

template<class Signature>
class CDelegate;

template<class R, class... Args>
class CDelegate<R(Args...)>
{
public:
	CDelegate() noexcept = default;

	bool IsBound() const noexcept { return m_bBound; }
	void Bind(std::function<R(Args...)> func) noexcept;
	void Unbind() noexcept;

	R operator()(Args... args) const
	{
		return m_Func(std::forward<Args>(args)...);
	}
private:
	bool m_bBound = { false };
	std::function<R(Args...)> m_Func;
};

template<class R, class ...Args>
inline void CDelegate<R(Args...)>::Bind(std::function<R(Args...)> func) noexcept
{
	m_Func = std::move(func);
	m_bBound = static_cast<bool>(m_Func);
}

template<class R, class ...Args>
inline void CDelegate<R(Args...)>::Unbind() noexcept
{
	m_Func = {};
	m_bBound = false;
}


NS_END