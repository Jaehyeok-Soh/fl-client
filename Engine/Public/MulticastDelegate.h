#pragma once
#include "Base.h"

NS_BEGIN(Engine)

template<class T>
class CMulticastDelegate;

template<class... Args>
class CMulticastDelegate<void(Args...)>
{
	// 자동 해제용 Thunk
	static void Unsubscribe_Thunk(void* pOwner, DelegateHandle _handle) noexcept
	{
		static_cast<CMulticastDelegate*>(pOwner)->Unsubscribe(_handle);
	}
	struct Slot
	{
		bool bAlive = { false };
		DelegateHandle key = {};
		std::function<void(Args...)> func;
	};
public:
	CMulticastDelegate()
	{
		m_vecSlots.reserve(50);
		m_vecPending.reserve(20);
	}
	DelegateHandle Subscribe(std::function<void(Args...)> func) noexcept;
	bool Unsubscribe(DelegateHandle _handle) noexcept;
	void Clear() noexcept;

	// 타입일치 검사위해 A
	template<typename... A>
	void Broadcast(A&&... args)
	{
		static_assert(sizeof...(A) == sizeof...(Args), "Broadcast argument count must match the delegate signature");

		m_iBroadcastDepth++;
		for (size_t i = 0; i < m_vecSlots.size(); ++i)
		{
			if (!m_vecSlots[i].bAlive)
				continue;

			m_vecSlots[i].func(std::forward<A>(args)...);
		}
		m_iBroadcastDepth--;

		if (!IsBroadcasting())
			Apply_Pending();
	}

private:
	bool IsBroadcasting() const { return m_iBroadcastDepth > 0; }
	void Apply_Pending();
private:
	uint32_t m_iBroadcastDepth = { 0 };
	uint64_t m_iNextID = { 1 };
	std::vector<Slot> m_vecSlots;
	std::vector<Slot> m_vecPending;
};

template<class ...Args>
inline DelegateHandle CMulticastDelegate<void(Args...)>::Subscribe(std::function<void(Args...)> func) noexcept
{
	Slot slot = {};
	slot.bAlive = true;
	slot.key.iGeneration = 1;
	slot.key.iID = m_iNextID++;
	slot.func = std::move(func);

	if (IsBroadcasting())
		m_vecPending.emplace_back(std::move(slot));
	else
		m_vecSlots.emplace_back(std::move(slot));

	return slot.key;
}

template<class ...Args>
inline bool CMulticastDelegate<void(Args...)>::Unsubscribe(DelegateHandle _handle) noexcept
{
	bool bReturn = { false };

	if (IsBroadcasting())
	{
		for (auto& slot : m_vecSlots)
		{
			if (slot.bAlive && slot.key == _handle)
			{
				slot.bAlive = false;
				bReturn = true;
				break;
			}
		}

		for (auto& slot : m_vecPending)
		{
			if (slot.bAlive && slot.key == _handle)
			{
				slot.bAlive = false;
				bReturn = true;
				break;
			}
		}
	}
	else
	{
		auto itr = std::remove_if(m_vecSlots.begin(), m_vecSlots.end(),
			[&](const Slot& s) -> bool
			{
				return (s.bAlive && s.key == _handle);
			});

		if (itr != m_vecSlots.end())
		{
			bReturn = true;
			m_vecSlots.erase(itr, m_vecSlots.end());
		}
	}

	return bReturn;
}

template<class ...Args>
inline void CMulticastDelegate<void(Args...)>::Clear() noexcept
{
	if (IsBroadcasting())
	{
		for (auto& slot : m_vecSlots)
			slot.bAlive = false;
		for (auto& slot : m_vecPending)
			slot.bAlive = false;
	}
	else
	{
		m_vecSlots.clear();
		m_vecPending.clear();
	}
}

template<class ...Args>
inline void CMulticastDelegate<void(Args...)>::Apply_Pending()
{
	auto itr = std::remove_if(m_vecSlots.begin(), m_vecSlots.end(),
		[&](const Slot& s)->bool
		{
			return !s.bAlive;
		});

	m_vecSlots.erase(itr, m_vecSlots.end());

	for (auto& slot : m_vecPending)
	{
		if (slot.bAlive)
			m_vecSlots.emplace_back(std::move(slot));
	}
	m_vecPending.clear();
}

NS_END