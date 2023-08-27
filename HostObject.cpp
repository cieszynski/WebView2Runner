#include "HostObject.h"

HostObject::HostObject(RunCallbackAsync runCallbackAsync)
{
}

void HostObject::set_AppWindow(AppWindow *appWindow)
{
    m_appWindow = appWindow;
}

STDMETHODIMP_(HRESULT __stdcall) HostObject::get_Property(BSTR* stringResult)
{
    *stringResult = SysAllocString(L"Toast");
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) HostObject::put_Property(BSTR stringValue)
{
    if (m_appWindow != nullptr) {
        MessageBox(NULL, L"Richtig?", L"Frage:", NULL);
    }
    else
    {
        MessageBox(NULL, L"Falsch?", L"Frage:", NULL);
    }
    return S_OK;
}

// IDispatch implementation
STDMETHODIMP HostObject::GetTypeInfoCount(UINT* pctinfo)
{
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObject::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
    if (0 != iTInfo)
    {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib)
    {
        if (FAILED(LoadTypeLib(L"WebView2Runner.tlb", &m_typeLib))) {
            return 0;
        }
    }
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObject), ppTInfo);
}

STDMETHODIMP HostObject::GetIDsOfNames(
    REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
    if (FAILED(GetTypeInfo(0, lcid, &typeInfo))) {
        return 0;
    }
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObject::Invoke(
    DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
    VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
    Microsoft::WRL::ComPtr<ITypeInfo> typeInfo;
    if (FAILED(GetTypeInfo(0, lcid, &typeInfo))) {
        return 0;
    }
    return typeInfo->Invoke(
        this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}
