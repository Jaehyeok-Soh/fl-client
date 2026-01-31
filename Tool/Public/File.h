#pragma once
#include "Base.h"

NS_BEGIN(Tool)

class CFile :  public CBase
{
public:
	enum class FILE_TYPE
	{
		NONE, TEXTURE, MODEL, TXT, JSON, BIN, END
	};
	using CFILE = CFile;

public:
	explicit CFile();
	virtual ~CFile();
private:
	HRESULT	 Initialize(const wstring& wstrPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	CONST FILE_INFO&	Get_FileInfo()	CONST { return m_tInfo; }
	FILE_TYPE			Get_FileType()	CONST { return m_eFileType; }
private:
	FILE_INFO			m_tInfo;
	FILE_TYPE			m_eFileType;
public:
	static  CFile*		Create(const wstring& wstrPath, ID3D11Device* pDevice,ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END

