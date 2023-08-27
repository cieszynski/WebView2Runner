#pragma once
#include <functional>

#include <map>
#include <string>
#include <wrl\client.h>
#include "framework.h"
#include "HostObject_h.h"

#include "AppWindow.h"

class HostObject : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    IHostObject, IDispatch>
{
public:
    typedef std::function<void(void)> Callback;
    typedef std::function<void(Callback)> RunCallbackAsync;

    HostObject(RunCallbackAsync runCallbackAsync);

    void set_AppWindow(AppWindow *appWindow);

    STDMETHODIMP get_Property(BSTR* stringResult) override;
    STDMETHODIMP put_Property(BSTR stringValue) override;

    // IDispatch implementation
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    STDMETHODIMP GetIDsOfNames(
        REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

    STDMETHODIMP Invoke(
        DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
        VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

private:
    AppWindow *m_appWindow;

    Microsoft::WRL::ComPtr<ITypeLib> m_typeLib;
};

