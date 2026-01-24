#pragma once
#include "Base.h"
#include "Delegate.h"
#include "MulticastDelegate.h"

struct IChannel
{
	virtual ~IChannel() = default;
};

// 멀티캐스트 래핑
template<class Signature>
struct Channel : IChannel
{
	CMulticastDelegate<Signature> multicast;
};

NS_BEGIN(Engine)

struct Example { using Signature = void(_int i, _float f); };

// 위에서 선언한 구조체 Example
// using Signautre가 곧 아래 선언한 typename Tag
class CEventBus_Manager final : public CBase
{
	using Super = CBase;
private:
	CEventBus_Manager();
	virtual	~CEventBus_Manager() = default;
	
	HRESULT Initialize();
public:
	template<typename Tag, typename Func>
	DelegateHandle Add_Lambda(Func&& func)
	{
		using Sig = typename Tag::Signature;
		return Subscribe<Tag>(std::function<Sig>(std::forward<Func>(func)));
	}

	template<typename Tag>
	DelegateHandle Add_Static(typename Tag::Signature* pFunc)
	{
		using Sig = typename Tag::Signature;
		return Subscribe<Tag>(std::function<Sig>(pFunc));
	}

	template<typename Tag, typename T, typename... Args>
	DelegateHandle Add_MemFunc(T* pObj, void (T::* memFunc)(Args...))
	{
		using Sig = typename Tag::Signature;
		static_assert(std::is_same_v<Sig, void(Args...)>, "signature not match");
		return Subscribe<Tag>([pObj, memFunc](Args... args)
			{
				(pObj->*memFunc)(std::forward<Args>(args)...);
			});
	}

	template<typename Tag, typename T, typename... Args>
	DelegateHandle Add_MemFunc(T* pObj, void(T::*memFunc)(Args...) const)
	{
		using Sig = typename Tag::Signature;
		static_assert(std::is_same_v<Sig, void(Args...)>, "signature not match");
		return Subscribe<Tag>([pObj, memFunc](Args... args)
			{
				(pObj->*memFunc)(std::forward<Args>(args)...);
			});
	}

	template<typename Tag>
	bool Unsubscribe(DelegateHandle handle)
	{
		auto itr = m_channels.find(std::type_index(typeid(Tag)));
		if (itr == m_channels.end()) return false;

		auto* pChannel = static_cast<Channel<typename Tag::Signature>*>(itr->second);
		return pChannel->multicast.Unsubscribe(handle);
	}

	template<typename Tag, typename... Arg>
	void Broadcast(Arg&&... a)
	{
		auto itr = m_channels.find(std::type_index(typeid(Tag)));
		if (itr == m_channels.end()) return;

		auto* pChannel = static_cast<Channel<typename Tag::Signature>*>(itr->second);
		pChannel->multicast.Broadcast(std::forward<Arg>(a)...);
	}

	template<class Tag>
	void Clear_Channel()
	{
		auto itr = m_channels.find(std::type_index(typeid(Tag)));
		if (itr == m_channels.end()) return;

		auto* pChannel = static_cast<Channel<typename Tag::Signature>*>(itr->second);
		pChannel->multicast.Clear();
	}

	void Clear_All();
private:
	template<typename Tag>
	DelegateHandle Subscribe(std::function<typename Tag::Signature> func)
	{
		auto* channel = GetOrCreate<Tag>();
		return channel->multicast.Subscribe(std::move(func));
	}

	template<typename Tag>
	Channel<typename Tag::Signature>* GetOrCreate()
	{
		const std::type_index key{ typeid(Tag) };
		auto itr = m_channels.find(key);
		if (itr != m_channels.end())
			return static_cast<Channel<typename Tag::Signature>*>(itr->second);

		auto* pChannel = new Channel<typename Tag::Signature>();
		m_channels.emplace(key, pChannel);
		return pChannel;
	}
private:
	std::unordered_map<std::type_index, IChannel*> m_channels;
public:
	static CEventBus_Manager* Create();
	virtual void Free() override;
};

NS_END