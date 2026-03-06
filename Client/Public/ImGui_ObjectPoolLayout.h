#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_ObjectPoolLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
    struct Track
    {
        std::vector<_float> samples;
        _int   write = 0;
        _float acc = 0.f;
        _float sample_interval = 0.10f; // 10Hz
        _float windowSec = 20.f;
        _bool  pause = false;

        void Reset()
        {
            samples.assign(300, 0.f);
            write = 0;
            acc = 0.f;
        }

        void Push(_float dt, _float v)
        {
            if (pause) return;
            acc += dt;
            if (acc < sample_interval) return;
            acc = 0.f;

            if (samples.empty())
                samples.assign(300, 0.f);

            samples[write] = v;
            write = (write + 1) % (_int)samples.size();
        }

        _float SpanSec() const
        {
            if (samples.empty()) return 0.f;
            return (_float)samples.size() * sample_interval;
        }
    };
private:
	CImGui_ObjectPoolLayout();
	virtual ~CImGui_ObjectPoolLayout() = default;
	HRESULT Initialize();
private:
    void DrawYAxisTicks_3Multiple(const ImVec2& pMin, const ImVec2& pMax, _int iCapacity);
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_ObjectPoolLayout* Create();
	virtual void Free() override;
};

NS_END