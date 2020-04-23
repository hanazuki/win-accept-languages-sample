// Sample application to display Preferred Languages settings in Windows 10

#pragma comment(lib,"runtimeobject")

#include <iostream>
#include <exception>
#include <string>
#include <vector>

#include <winstring.h>
#include <roapi.h>
#include <wrl.h>
#include <windows.system.userprofile.h>

struct ScopedWinrtInitializer {
    ScopedWinrtInitializer() {
        auto hr = RoInitialize(RO_INIT_MULTITHREADED);
        if (FAILED(hr)) {
            throw std::runtime_error("RoInitialize failed");
        }
    }
    ScopedWinrtInitializer(ScopedWinrtInitializer const&) = delete;
    ~ScopedWinrtInitializer() {
        RoUninitialize();
    }
};

std::vector<std::wstring> preferredLanguages() {
    using namespace ABI::Windows::System::UserProfile;
    using Microsoft::WRL::ComPtr;

    HSTRING klass;
    auto hr = WindowsCreateString(RuntimeClass_Windows_System_UserProfile_GlobalizationPreferences,
        lstrlenW(RuntimeClass_Windows_System_UserProfile_GlobalizationPreferences),
        &klass);
    if (FAILED(hr)) {
        throw std::runtime_error("WindowsCreateString failed");
    }

    ComPtr<IGlobalizationPreferencesStatics> prefs;
    hr = RoGetActivationFactory(klass, IID_PPV_ARGS(&prefs));
    if (FAILED(hr)) {
        throw std::runtime_error("RoGetActivationFactory failed");
    }

    ABI::Windows::Foundation::Collections::IVectorView<HSTRING> *langs;
    hr = prefs->get_Languages(&langs);
    if (FAILED(hr)) {
        throw std::runtime_error("GlobalizationPreferences::get_Languages failed");
    }

    unsigned size;
    hr = langs->get_Size(&size);
    if (FAILED(hr)) {
        throw std::runtime_error("get_Size failed");
    }

    std::vector<std::wstring> result;
    for (unsigned i = 0; i < size; ++i) {
        HSTRING s;
        hr = langs->GetAt(i, &s);
        if (SUCCEEDED(hr)) {
            UINT32 len;
            auto p = WindowsGetStringRawBuffer(s, &len);
            result.emplace_back(p, len);
        }
    }

    return result;
}

int main() {
    ScopedWinrtInitializer ro;

    for(auto const lang: preferredLanguages()) {
        std::wcout << lang << std::endl;
    }
}
