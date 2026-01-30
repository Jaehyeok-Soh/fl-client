#include "pch.h"
#include "Folder.h"
#include "Engine_Utils.h"
#include "File.h"
#include "GameInstance.h"

CFolder::CFolder()
	: CBase(), m_pGameInstance(CGameInstance::GetInstance())
     , m_pContext(nullptr) , m_pDevice(nullptr) , m_pParentFloder(nullptr)
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFolder::Initialize(CFolder* pParentFloder, const wstring& wstrRootPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    /* 파일 이름이 있으면 FileName을 제거한다 */
    wstring wstr = Engine_Utils::NormalizePath_WString(std::filesystem::canonical(wstrRootPath));

    m_tInfo.wstrFloderFullPath = Engine_Utils::ToWString(
        Engine_Utils::NormalizePath(std::filesystem::canonical(wstrRootPath))).c_str();
    m_tInfo.wstrFloderName = path(wstrRootPath).filename();

    /* ParentFloder Setting 나를 가지고있는 폴더의 포인터*/
    /* 약한참조 레퍼런스 증가 x */
    if (pParentFloder == nullptr)
        m_pParentFloder = pParentFloder;

    for (auto& Path : std::filesystem::directory_iterator(m_tInfo.wstrFloderFullPath))
    {
        /* 현재 폴더를 도는데 아마 typde 이 폴더라면? 돌아보자.. */
        path CurPath = Engine_Utils::NormalizePath(Path.path());
        /*  폴더일때 */
        if (std::filesystem::is_directory(CurPath))
        {
            ++m_tInfo.iCountFloders;
            CFolder* pFloder = CFolder::Create(this, wstring(CurPath.wstring()), pDevice, pContext);
            if (pFloder == nullptr) return E_FAIL;
            m_mapTreeFloder.emplace(pFloder->m_tInfo.wstrFloderName, pFloder);
        }
        /* 파일 일때 */
        else
        {
            ++m_tInfo.iCountFiles;
            CFile* pFile = CFile::Create(CurPath, pDevice, pContext);
            if (pFile == nullptr) return E_FAIL;
                m_vecFile.push_back(pFile);
        }
    }

    path Path(wstrRootPath);

    return S_OK;
}

void CFolder::ImGui_Update()
{

}

vector<wstring> CFolder::Find_File(const wstring& wstrFileName)
{
    vector<wstring> wstrPreFindFile{};

    if (wstrFileName.empty()) return wstrPreFindFile;

    for (auto& FolderPair : m_mapTreeFloder)
    {
        CFolder* pFolder = FolderPair.second;
        if (!pFolder)
            continue;
        vector<wstring> wstrCurFindFile = pFolder->Find_File(wstrFileName);
        if (wstrCurFindFile.empty())
            continue;
        wstrPreFindFile.insert( wstrPreFindFile.end(), wstrCurFindFile.begin() , wstrCurFindFile.end());
    }


    for (auto& File : m_vecFile)
    {
        if (!File)
            continue;

        if (File->Get_FileInfo().wstrFileName.find(wstrFileName) == std::wstring::npos)
            continue;

        wstrPreFindFile.push_back(File->Get_FileInfo().wstrFileFullPath);
    } 

    return wstrPreFindFile;
}

CFolder* CFolder::Create(CFolder* pParentFloder, const std::wstring& wstrRootPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFolder* pFolder = new CFolder;

    if (FAILED(pFolder->Initialize(pParentFloder, wstrRootPath, pDevice, pContext)))
    {
        Safe_Release(pFolder);
        MSG_BOX("Folder is Faield to Create");
        return nullptr;
    }
    return pFolder;
}

void CFolder::Free()
{
    __super::Free();

    /* 파일정리 */
    for (auto& CFile : m_vecFile)
        Safe_Release(CFile);
    /* 폴더 정리 */
    for (auto& Floder : m_mapTreeFloder)
        Safe_Release(Floder.second);


    Safe_Release(m_pGameInstance);
    Safe_Release(m_pParentFloder);
    return;
}
