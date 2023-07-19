#include <iostream>
#include <windows.h>
#include <nvapi.h>

// Simple script that limits GPU usage. Works great for hidden bitcoin mining scripts. 

#pragma comment(lib, "nvapi.lib")

// Function to set GPU usage to a specific percentage
bool SetGPUUsageLimit(int gpuIndex, int percentage) {
    NvPhysicalGpuHandle nvGPUHandle;
    NvU32 domainId;
    NvU32 subDomainId;
    NV_GPU_DYNAMIC_PSTATES_INFO_EX dynamicStatesInfo = {0};

    NVAPI_Status nvStatus = NVAPI_OK;

    nvStatus = NvAPI_Initialize();
    if (nvStatus != NVAPI_OK) {
        std::cerr << "NVAPI initialization failed" << std::endl;
        return false;
    }

    nvStatus = NvAPI_GPU_GetUsages(nullptr, &dynamicStatesInfo);
    if (nvStatus != NVAPI_OK) {
        std::cerr << "NVAPI GPU GetUsages failed" << std::endl;
        NvAPI_Unload();
        return false;
    }

    // Ensure the provided GPU index is valid
    if (gpuIndex >= dynamicStatesInfo.usagesCount) {
        std::cerr << "Invalid GPU index" << std::endl;
        NvAPI_Unload();
        return false;
    }

    // Get the GPU handle and domain and subdomain IDs
    nvGPUHandle = dynamicStatesInfo.usages[gpuIndex].physicalGpuHandle;
    domainId = dynamicStatesInfo.usages[gpuIndex].domainId;
    subDomainId = dynamicStatesInfo.usages[gpuIndex].subDomainId;

    // Get the current dynamic P-states info
    dynamicStatesInfo.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
    nvStatus = NvAPI_GPU_GetDynamicPstatesInfoEx(nvGPUHandle, &dynamicStatesInfo);
    if (nvStatus != NVAPI_OK) {
        std::cerr << "NVAPI GPU GetDynamicPstatesInfoEx failed" << std::endl;
        NvAPI_Unload();
        return false;
    }

    // Set the desired target usage percentage
    dynamicStatesInfo.pstates[subDomainId].percentage = percentage;

    // Apply the new settings
    nvStatus = NvAPI_GPU_SetDynamicPstatesInfoEx(nvGPUHandle, &dynamicStatesInfo);
    if (nvStatus != NVAPI_OK) {
        std::cerr << "NVAPI GPU SetDynamicPstatesInfoEx failed" << std::endl;
        NvAPI_Unload();
        return false;
    }

    NvAPI_Unload();
    return true;
}

int main() {
    int gpuIndex = 0; // Change this to the appropriate GPU index on your system
    int targetUsage = 5; // The desired GPU usage percentage (5% in this case)

    if (SetGPUUsageLimit(gpuIndex, targetUsage)) {
        std::cout << "GPU usage successfully limited to " << targetUsage << "%" << std::endl;
    } else {
        std::cerr << "Failed to limit GPU usage" << std::endl;
    }

    return 0;
}
