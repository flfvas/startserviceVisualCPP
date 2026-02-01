#include <windows.h>
#include <iostream>

// 强制链接器使用控制台子系统
#pragma comment(linker, "/subsystem:console")

/*===========================
模块：Virtual Disk 服务控制模块
功能：检查并启动系统的虚拟磁盘服务 (vds)
===========================*/

bool StartVirtualDiskService()
{
    SC_HANDLE hSCM = NULL;
    SC_HANDLE hService = NULL;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    bool bResult = false;

    // 1. 打开服务控制管理器
    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (NULL == hSCM)
    {
        return false;
    }

    // 2. 打开 Virtual Disk 服务 (服务名为 "vds")
    hService = OpenService(hSCM, L"vds", SERVICE_START | SERVICE_QUERY_STATUS);
    if (NULL == hService)
    {
        CloseServiceHandle(hSCM);
        return false;
    }

    // 3. 检查服务状态
    if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
    {
        if (ssStatus.dwCurrentState == SERVICE_RUNNING || ssStatus.dwCurrentState == SERVICE_START_PENDING)
        {
            bResult = true;
        }
        else
        {
            // 4. 尝试启动服务
            if (StartService(hService, 0, NULL))
            {
                bResult = true;
            }
        }
    }

    // 清理资源
    if (hService)
    {
        CloseServiceHandle(hService);
    }
    if (hSCM)
    {
        CloseServiceHandle(hSCM);
    }

    return bResult;
}

int main()
{
    if (StartVirtualDiskService())
    {
        std::cout << "Virtual Disk 服务启动成功或已在运行。" << std::endl;
    }
    else
    {
        std::cerr << "启动失败。请确保以管理员身份运行程序。" << std::endl;
    }
    return 0;
}