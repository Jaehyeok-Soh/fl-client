#include "Engine_pch.h"
#include "ThreadWorker.h"
#include "ThreadPool.h"

CThreadWorker::CThreadWorker(CThreadPool* pOwnerPool)
	: m_pOwnerPool(pOwnerPool)
{
	
}

// 생성 즉시 함수를 정의 한다. 
// 뮤텍스를 획득하여 lock을 생성한다.
// 리소스에 구애받지 않도록 lock을 생성하면 뮤텍스가 획득되고, 락이 해제되는 즉시 뮤텍스도 반환된다.
// 프로그램이 종료될때까지 수행해야하므로 ShutdownRequested가 있거나 job이 비어있기전까지 계속 기다린다.
// 조건변수(condition_variable)은 처음에 생성한 락을 획득하고, 락이 해제되면 뮤텍스를 반환한다.
// 조건 변수가 적용되면 스레드는 작업을 중지하고, 다른 스레드의 로직에 의해 다시 깨어날때까지 뮤텍스를 반환한ㄷ,.
// 만약 스레드가 깨어나면 조건변수를 다시 확인하고(프로그램 종료, queue가 비어있지 않은 경우)
// 참일경우 계속 실행 아니면 Sleep 상태
// ★ 조건변수에 대한 불필요한 깨어남 (spurious wakeups)라는 문제가 있는데, 이는 매우 성가신 문제이다.
// 스레드가 아무도 꺠우라고 요ㅕ청하지 않았는데도 깨어나는 경우가 발생할 수 있다.
// 그래서 이 조건문 안에 올바른 조건을 확인하는 코드를 넣어야한다.
// 아무튼 조건이 참이되어 깨어나고 queue가 비어있지않다면 일을 수행한다.
// unlock! 함수실행! lock! --> 다른 스레드가 실행을 기다리며 Block 되지 않기 위함
void CThreadWorker::operator()()
{
	std::unique_lock<mutex> lock(m_pOwnerPool->m_mutex);
	while (m_pOwnerPool->m_bShutdown_Requested == false ||
		((m_pOwnerPool->m_bShutdown_Requested == true && m_pOwnerPool->m_queJobs.empty() == false)))
	{
		m_pOwnerPool->m_iBusyThreads--;
		m_pOwnerPool->m_convar.wait(lock, [this] {
			return (this->m_pOwnerPool->m_bShutdown_Requested == true) || (this->m_pOwnerPool->m_queJobs.empty() == false);
		});
		m_pOwnerPool->m_iBusyThreads++;

		if (this->m_pOwnerPool->m_queJobs.empty() == false)
		{
			auto func = m_pOwnerPool->m_queJobs.front();
			m_pOwnerPool->m_queJobs.pop();

			lock.unlock();
			func();
			lock.lock();
		}
	}
}
