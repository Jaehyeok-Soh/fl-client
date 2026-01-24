#include "Engine_pch.h"
#include "Engine_Utils.h"

_bool Engine_Utils::StartsWith(const string& str, const string& comp)
{
    string::size_type iIndex = str.find(comp);
    if ((iIndex != string::npos) && (iIndex == 0))
        return true;

    return false;
}

_bool Engine_Utils::StartsWith(const wstring& wstr, const wstring& comp)
{
    wstring::size_type iIndex = wstr.find(comp);
    if ((iIndex != wstring::npos) && (iIndex == 0))
        return true;

    return false;
}

void Engine_Utils::Replace(OUT string& str, string comp, string rep)
{
    string temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}

void Engine_Utils::Replace(OUT wstring& str, wstring comp, wstring rep)
{
    wstring temp = str;

    size_t start_pos = 0;
    while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, comp.length(), rep);
        start_pos += rep.length();
    }

    str = temp;
}

wstring Engine_Utils::ToWString(string value)
{
    return wstring(value.begin(), value.end());
}

string Engine_Utils::ToString(wstring value)
{
    return string(value.begin(), value.end());
}
