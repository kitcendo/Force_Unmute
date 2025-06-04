// Force_Unmute_v6.6.cpp
// incace HDMI ghost / D3Cold / Padding ghost / Timing sensitive / No terminate
// include try/catch
// Turn Off QuickEdit

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys.h>
#include <initguid.h>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#define SAFE_RELEASE(p) if (p) { (p)->Release(); (p) = nullptr; }

void SleepMs(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// === ReEnumDevices ===
std::vector<std::wstring> ReEnumDevices()
{
    std::vector<std::wstring> deviceList;
    try
    {
        CoInitialize(nullptr);
        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDeviceCollection* pCollection = nullptr;

        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        if (FAILED(hr))
            throw std::runtime_error("CoCreateInstance failed");

        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr))
            throw std::runtime_error("EnumAudioEndpoints failed");

        UINT count = 0;
        pCollection->GetCount(&count);

        for (UINT i = 0; i < count; ++i)
        {
            IMMDevice* pDevice = nullptr;
            IPropertyStore* pStore = nullptr;

            hr = pCollection->Item(i, &pDevice);
            if (FAILED(hr))
                continue;

            hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
            if (FAILED(hr))
            {
                SAFE_RELEASE(pDevice);
                continue;
            }

            PROPVARIANT varName;
            PropVariantInit(&varName);

            hr = pStore->GetValue(PKEY_Device_FriendlyName, &varName);
            if (SUCCEEDED(hr))
            {
                deviceList.push_back(varName.pwszVal);
            }

            PropVariantClear(&varName);
            SAFE_RELEASE(pStore);
            SAFE_RELEASE(pDevice);
        }

        SAFE_RELEASE(pCollection);
        SAFE_RELEASE(pEnumerator);
        CoUninitialize();
    }
    catch (const std::exception& ex)
    {
        std::wcout << L"[ERROR] Exception in ReEnumDevices: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::wcout << L"[ERROR] Unknown exception in ReEnumDevices" << std::endl;
    }

    return deviceList;
}

// === GetMuteStateSafe ===
bool GetMuteStateSafe(const std::wstring& deviceName)
{
    bool mute = false;
    try
    {
        CoInitialize(nullptr);
        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDeviceCollection* pCollection = nullptr;

        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        if (FAILED(hr))
            throw std::runtime_error("CoCreateInstance failed");

        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr))
            throw std::runtime_error("EnumAudioEndpoints failed");

        UINT count = 0;
        pCollection->GetCount(&count);

        for (UINT i = 0; i < count; ++i)
        {
            IMMDevice* pDevice = nullptr;
            IPropertyStore* pStore = nullptr;

            hr = pCollection->Item(i, &pDevice);
            if (FAILED(hr))
                continue;

            hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
            if (FAILED(hr))
            {
                SAFE_RELEASE(pDevice);
                continue;
            }

            PROPVARIANT varName;
            PropVariantInit(&varName);

            hr = pStore->GetValue(PKEY_Device_FriendlyName, &varName);
            if (SUCCEEDED(hr))
            {
                if (deviceName == varName.pwszVal)
                {
                    IAudioEndpointVolume* pVolume = nullptr;
                    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&pVolume);
                    if (SUCCEEDED(hr))
                    {
                        BOOL bMute = FALSE;
                        pVolume->GetMute(&bMute);
                        mute = (bMute != FALSE);
                        SAFE_RELEASE(pVolume);
                    }
                }
            }

            PropVariantClear(&varName);
            SAFE_RELEASE(pStore);
            SAFE_RELEASE(pDevice);
        }

        SAFE_RELEASE(pCollection);
        SAFE_RELEASE(pEnumerator);
        CoUninitialize();
    }
    catch (const std::exception& ex)
    {
        std::wcout << L"[ERROR] Exception in GetMuteStateSafe: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::wcout << L"[ERROR] Unknown exception in GetMuteStateSafe" << std::endl;
    }

    return mute;
}

// === SetMuteFalse ===
bool SetMuteFalse(const std::wstring& deviceName)
{
    bool success = false;
    try
    {
        CoInitialize(nullptr);
        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDeviceCollection* pCollection = nullptr;

        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        if (FAILED(hr))
            throw std::runtime_error("CoCreateInstance failed");

        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        if (FAILED(hr))
            throw std::runtime_error("EnumAudioEndpoints failed");

        UINT count = 0;
        pCollection->GetCount(&count);

        for (UINT i = 0; i < count; ++i)
        {
            IMMDevice* pDevice = nullptr;
            IPropertyStore* pStore = nullptr;

            hr = pCollection->Item(i, &pDevice);
            if (FAILED(hr))
                continue;

            hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
            if (FAILED(hr))
            {
                SAFE_RELEASE(pDevice);
                continue;
            }

            PROPVARIANT varName;
            PropVariantInit(&varName);

            hr = pStore->GetValue(PKEY_Device_FriendlyName, &varName);
            if (SUCCEEDED(hr))
            {
                if (deviceName == varName.pwszVal)
                {
                    IAudioEndpointVolume* pVolume = nullptr;
                    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&pVolume);
                    if (SUCCEEDED(hr))
                    {
                        hr = pVolume->SetMute(FALSE, nullptr);
                        success = SUCCEEDED(hr);
                        SAFE_RELEASE(pVolume);
                    }
                }
            }

            PropVariantClear(&varName);
            SAFE_RELEASE(pStore);
            SAFE_RELEASE(pDevice);
        }

        SAFE_RELEASE(pCollection);
        SAFE_RELEASE(pEnumerator);
        CoUninitialize();
    }
    catch (const std::exception& ex)
    {
        std::wcout << L"[ERROR] Exception in SetMuteFalse: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::wcout << L"[ERROR] Unknown exception in SetMuteFalse" << std::endl;
    }

    return success;
}

// === Main ===
int main()
{
    std::wcout << L"=== Force_Unmute_v6.6 START ===" << std::endl;
    
    // Turn Off QuickEdit Mode
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hStdin, mode);

    // Wait 10 seconds
    std::wcout << L"[INFO] Waiting 10 seconds for Windows to detect devices..." << std::endl;
    SleepMs(10000);

    std::vector<std::wstring> devices = ReEnumDevices();

    std::wcout << L"[INFO] Found " << devices.size() << L" active playback devices." << std::endl;
    for (size_t i = 0; i < devices.size(); ++i)
    {
        std::wcout << L"[ENUM] Device " << i + 1 << L": " << devices[i] << std::endl;
    }

    // === Professional Loop ===
    while (true)
    {
        bool allUnmuted = true;

        for (const auto& device : devices)
        {
            std::wcout << L"[PROCESS] " << device << L" ..." << std::endl;

            try
            {
                std::wcout << L"[STEP 1] Check mute state ..." << std::endl;
                bool isMute = GetMuteStateSafe(device);
                if (isMute)
                {
                    std::wcout << L"[STATUS] MUTE = YES" << std::endl;
                    std::wcout << L"[STEP 2] Unmuting ..." << std::endl;
                    bool success = SetMuteFalse(device);
                    if (success)
                    {
                        std::wcout << L"[STEP 2] Unmute OK" << std::endl;
                    }
                    else
                    {
                        std::wcout << L"[STEP 2] Unmute FAILED" << std::endl;
                    }

                    // === Recheck after Unmute ===
                    SleepMs(1000);
                    bool isMuteAfter = GetMuteStateSafe(device);
                    if (isMuteAfter)
                    {
                        std::wcout << L"[STATUS] STILL MUTE → Will retry" << std::endl;
                        allUnmuted = false;
                    }
                    else
                    {
                        std::wcout << L"[STATUS] NOW UNMUTED" << std::endl;
                    }
                }
                else
                {
                    std::wcout << L"[STATUS] MUTE = NO" << std::endl;
                }
            }
            catch (const std::exception& ex)
            {
                std::wcout << L"[ERROR] Exception in main loop: " << ex.what() << std::endl;
            }
            catch (...)
            {
                std::wcout << L"[ERROR] Unknown exception in main loop" << std::endl;
            }
        }

        if (allUnmuted)
        {
            std::wcout << L"[SUCCESS] All devices unmuted. Showing status 10 sec ..." << std::endl;
            for (const auto& device : devices)
            {
                bool isMute = GetMuteStateSafe(device);
                std::wcout << L"[FINAL STATUS] " << device << L": " << (isMute ? L"MUTE" : L"UNMUTE") << std::endl;
            }

            SleepMs(10000);
            std::wcout << L"=== Force_Unmute_v6.6 DONE ===" << std::endl;
            break;
        }
        else
        {
            std::wcout << L"[LOOP] Some devices still muted → Retrying in 2 sec ..." << std::endl;
            SleepMs(2000);
        }
    }

    return 0;
}