#include <windows.h>
#include <iostream>

// 强制链接器使用控制台子系统，解决 WinMain 符号未定义问题
#pragma comment(linker, "/subsystem:console")

/*===========================
模块：Print Spooler 服务控制模块
功能：检查并启动系统的打印机后台处理程序
===========================*/

bool StartPrintSpooler()
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

    // 2. 打开 Print Spooler 服务 (服务名为 "Spooler")
    hService = OpenService(hSCM, L"Spooler", SERVICE_START | SERVICE_QUERY_STATUS);
    if (NULL == hService)
    {
        CloseServiceHandle(hSCM);
        return false;
    }

    // 3. 检查服务状态，如果已经运行则无需启动
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
    if (StartPrintSpooler())
    {
        std::cout << "Print Spooler 服务启动成功或已在运行。" << std::endl;
    }
    else
    {
        std::cerr << "启动失败。请检查是否具有管理员权限。" << std::endl;
    }
    return 0;
}