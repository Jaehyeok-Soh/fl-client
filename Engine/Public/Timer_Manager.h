#pragma once
#include "Base.h"

//
// TODO - TimeScale 구조
// ObjectManager에서 뿌려주는 deltatime을 각 레이어별로 구분해서 뿌려주는식
// 예를들어 UI는 정상작동, Player는 0.5배, Enemy_Beach는 0.1배, 혹은 어떤것은 Puase
// 이런식으로 확장적인 연출이 가능하기 때문이다.
// 여기서 유의 해야할건 물리는 고정틱으로, 연출쪽만 스케일타임으로 지정해주어야한다.
// 또 !! 유의 해야할건 물리는 모두가 동일한 고정틱으로 연산하되, 연출을 위한 입력, 외력 등은 Scale이 적용되어야함
// text파일로 의사코드 정리해놓겠음 이부분 참고하여 추후에 구현
// DeltaTime은 고정된 틱을 분배, 각 레이어별로 구분된 Scale을 건내주고 각 함수 초반에서
// Unscaled DeltaTime과 DeltaTime을 구분해서 적용하는 식으로하면 Overhead도 적다고 한다.
// 이것의 장점은 각 로직에서 Scale을 적용시킬것과 적용시키지 않을것을 선택할 수 있게 된다. 특히 물리쪽에서 중요해보인다.
// 그래서 나는 CTimer를 고정적으로 60프레임을 뿌려주는 용도로만 사용, TimerManager에서는 각 Layer별 TimeSclae을 관리하고 파라미터로 넘겨주는 역할을
// 하게 할것이며, 현재 학원 구조에서 Timer를 따로 쿨타임 용도로 쓰는 구조는 피하고, 유틸 구조체를 선언해서 파라미터로 넘겨져 오는 Deltatime 및 Sclae
// 기반으로 연산을 진행할 예정이다.
// 물리에서 적용될 Scale을 구분하는것이 아직 미숙하니 이부분은 유의하면서 추후에 구현할것.
//


NS_BEGIN(Engine)

class CTimer;

class CTimer_Manager final : public CBase
{
	using Super = CBase;
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

	HRESULT					Initialize();
public:
	_float					Get_TimeDelta(const _tchar* pTimerTag);
	void					Set_MaxTimeDelta(const _tchar* pTimerTag, _float fMaxTimeDelta);
public:
	HRESULT					Add_Timer(const _tchar* pTimerTag);
	void					Remove_Timer(const _tchar* pTimerTag);
	void					Compute_TimeDelta(const _tchar* pTimerTag);
	void					Clear_Timers();
	CTimer*					Find_Timer(const _tchar* pTimerTag);
private:
	map<const _wstring, CTimer*> m_mapTimers;
public:
	virtual void			Free() override;
	static CTimer_Manager*	Create();
};

NS_END
