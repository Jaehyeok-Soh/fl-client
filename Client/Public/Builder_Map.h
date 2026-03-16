#pragma once
#include "BuilderBase.h"
#include "DataStruct_Map.h"
#include "Monster_Base.h"

NS_BEGIN(Engine)

class CShader;

NS_END

NS_BEGIN(Client)

class CBuilder_Map : public CBuilderBase
{
	using Super = CBuilderBase;
private:
	CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual ~CBuilder_Map() = default;
private:
	HRESULT		Initialize();
public:
	virtual HRESULT Build(const CDataDocumentBase& document) override;
private:

	HRESULT LevelData_Setting(const DTO::TLevelData& tData);
	HRESULT Create_StaticObject(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_LandScape(const DTO::TMap_MapObjectData& tData);

	/* Plants */
	HRESULT Create_Bush(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Grass(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Moss(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Tree(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Vine(const DTO::TMap_MapObjectData& tData);
	/* ------ */
	HRESULT Create_Water(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Rock(const DTO::TMap_MapObjectData& tData);
	HRESULT Create_Fog(const DTO::TMap_MapObjectData& tData);

	/* Batch Player */
	HRESULT Batch_Player(const DTO::TMap_MapObjectData& tData);
	/* Batch Monster */
	HRESULT	Batch_Monster(const DTO::TMap_MapObjectData& tData);
	/* Batch Object */
	HRESULT	Batch_Object(const DTO::TMap_MapObjectData& tData);

	/* Trigger Box */
	HRESULT	Create_TriggerBox_ChangeLevel(const DTO::TMap_MapObjectData& tData);
	HRESULT	Create_TriggerBox_MonsterSpawner(const DTO::TMap_MapObjectData& tData);
	HRESULT	Create_TriggerBox_GlobalEvent_BroadCaster(const DTO::TMap_MapObjectData& tData);
	HRESULT	Create_TriggerBox_TutorialUIEvent(const DTO::TMap_MapObjectData& tData);


	/* Invisible Wall */
	HRESULT	Create_InvisibleWall(const DTO::TMap_MapObjectData& tData);


private:
	CShader*		m_pMeshShader{nullptr};
	CShader*		m_pInstMeshShader{nullptr};

	ELevelType		m_eLevelType{};

	CGameInstance*	m_pGameInstance{ nullptr };
public:
	static EMonster_Type Change_MakeMonsterType_To_MonsterType(DTO::EMakeMonsterType eMakeMonsterType);
	static CBuilder_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID);
	virtual void Free() override;
};

NS_END
