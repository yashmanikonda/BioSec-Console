#include "GetProdID.h"
#include <comdef.h>
#include <Wbemidl.h>

#include <iostream> // Include for print statements

// Function to retrieve the product ID using WMI

std::wstring GetProdInfo::GetProductID() {
    CoInitializeEx(0, COINIT_MULTITHREADED);

    IWbemLocator* pLoc = nullptr;
    CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    IWbemServices* pSvc = nullptr;
    pLoc->ConnectServer(bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
    pLoc->Release();

    CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

    IEnumWbemClassObject* pEnumerator = nullptr;
    pSvc->ExecQuery(_bstr_t("WQL"), _bstr_t("SELECT * FROM Win32_OperatingSystem"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    std::wstring productID;

    while (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (uReturn == 0) {
            break;
        }

        VARIANT vtProp;
        pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
        productID = vtProp.bstrVal;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pSvc->Release();
    CoUninitialize();

    return productID;
}
