#pragma once
#include "Base.h"


// MainThread에서 std::packaged_task 생성, GetFuture()를 사용하여 퓨처를 가져온다.
// 그렇게 되면 패키지 태스크와 퓨처는 동일한 리소스를 공유한다.
// 그 후 스레드를 사용하여 패키지 테스트를 생성한 스레드로 이동시킨다.
// 
// 패키지 태스크와 해당 태스크에 작업함수(job function)을 할당한다.
// std::packaged_task<void(void)> task(job_function);
// 해당 태스크의 future를 가져오고, thread를 할당하여 task를 넘긴다.
// std::future future = task.get_future();
// std::thread thread(std::move(task));
// future.wait_for(std::chrono::seconds(1));
// 
// 스레드를 생성해서 태스크를 스레드로 이동시킨다. 테스크를 기다리는 스레드 세개가 있다면
// 테스크가 들어올 때 스레드 중 하나가 태스크를 가져와 처리하기 시작한다.
// 스레드가 없으면 태스크가 쌓이고, 스레드가 자신의 태스크를 완료하면 큐에서 태스크 하나를 가져와 처리하기 시작한다.
// 

NS_BEGIN(Engine)

class CThreadWorker;

class CThreadPool final : public CBase
{
	using Super = CBase;
private:
	CThreadPool();
	virtual ~CThreadPool() = default;
	HRESULT Initialize();
public:
	_uint Get_BusyThreads() const { return m_iBusyThreads; }
	template<typename FuncType, typename... Args>
	auto AddTask(FuncType&& f, Args&&... args) -> future<decltype(f(args...))>;
private:
	mutable mutex m_mutex;
	_int m_iBusyThreads = {0}; // 그래프 디버그용
	condition_variable m_convar;
	vector<thread> m_vecThreads;
	_bool m_bShutdown_Requested{ false };
	std::queue<std::function<void()>> m_queJobs;
public:
	static CThreadPool* Create();
	virtual void Free() override;
	
	// ThreadWorker는 Pool의 공유 뮤텍스와 공유큐에 접근해야하므로 열어준다.
	friend class CThreadWorker;
};

template<typename FuncType, typename ...Args>
inline auto CThreadPool::AddTask(FuncType&& f, Args && ...args) -> future<decltype(f(args ...))/*데칼 유형 함수*/>
{
	auto func = std::bind(std::forward<FuncType>(f), std::forward<Args>(args)...);

	// 복사 가능한 객체가 되기위해 make_shared를 한다.
	// 왜냐하면, 람다함수에 넣기 위해 복사를 할 수 있어야 하기 떄문!
	auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

	// 여기서 queue에 넣기위해 wrapper_func로 감싼다.
	// 왜냐하면 queue의 함수 자료형은 void()이기 떄문!
	auto wrapper_func = [task_ptr]() {(*task_ptr)(); };

	{
		// 래퍼 함수는 lock을 획득한 후 queue에 등록된다.
		// 큐에는 이미 다른 스레드가 접근해서 접근할 수 있기 때문이다.
		std::lock_guard<mutex> lock(m_mutex);
		m_queJobs.push(wrapper_func);
		// 그러고 조건변수를 활성화하여 단일 스레드를 깨운다.
		// 조건 변수를 기다리는 스레드가 아니면 해당 스레드가 깨어나고
		// 이제 각 태스크가 처리를 할 수있다.
		m_convar.notify_one();
	}

	// promise로부터 future를 반환
	return task_ptr->get_future();
}

NS_END
