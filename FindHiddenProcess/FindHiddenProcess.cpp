#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <iomanip>

#pragma comment(lib, "advapi32.lib")


/// <summary>
/// 启用 Token 的所有特权
/// </summary>
/// <param name="hToken">目标 Token。可以为空（如果为空则自动使用当前进程的）。</param>
/// <returns>是否成功。</returns>
BOOL EnableAllPrivileges(HANDLE hToken) {
	BOOL bResult = FALSE;
	HANDLE hTokenLocal = nullptr;
	DWORD dwTokenInfoSize = 0;
	PTOKEN_PRIVILEGES pTokenPrivileges = nullptr;

	// 处理令牌句柄
	if (hToken == nullptr) {
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hTokenLocal)) {
			return FALSE;
		}
		hToken = hTokenLocal;
	}

	// 获取所需缓冲区大小
	if (!GetTokenInformation(hToken, TokenPrivileges, nullptr, 0, &dwTokenInfoSize) &&
		GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		goto cleanup;
	}

	// 分配权限信息缓冲区
	pTokenPrivileges = reinterpret_cast<PTOKEN_PRIVILEGES>(malloc(dwTokenInfoSize));
	if (!pTokenPrivileges) {
		goto cleanup;
	}

	// 获取实际权限信息
	if (!GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwTokenInfoSize, &dwTokenInfoSize)) {
		goto cleanup;
	}

	// 启用所有权限
	bResult = TRUE;
	for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i) {
		LUID_AND_ATTRIBUTES& la = pTokenPrivileges->Privileges[i];
		TOKEN_PRIVILEGES tp = { 1, { { la.Luid, SE_PRIVILEGE_ENABLED } } };

		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
			bResult = FALSE;
		}
		else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
			bResult = FALSE;
		}
	}

cleanup:
	if (pTokenPrivileges) free(pTokenPrivileges);
	if (hTokenLocal) CloseHandle(hTokenLocal);
	return bResult;
}


// 函数声明
std::unordered_set<DWORD> ScanPotentialProcesses();
std::unordered_set<DWORD> GetNormalProcessList();
void AnalyzeHiddenProcesses(const std::unordered_set<DWORD>& potentialProcesses);

int main() {
    EnableAllPrivileges(0);

    std::cout << "Starting hidden process detection..." << std::endl;
    
    // 第一步：慢速扫描所有可能的PID
    std::cout << "Phase 1: Scanning all possible PIDs (8-65536)..." << std::endl;
    std::unordered_set<DWORD> potentialProcesses = ScanPotentialProcesses();
    
    // 第二步：快速获取正常进程列表
    std::cout << "\nPhase 2: Getting normal process list via Toolhelp32..." << std::endl;
    
    // 第三步：分析差异，找出隐藏进程
    std::cout << "\nPhase 3: Analyzing for hidden processes..." << std::endl;
    AnalyzeHiddenProcesses(potentialProcesses);
    
    return 0;
}

// 扫描所有可能的PID（慢速）
std::unordered_set<DWORD> ScanPotentialProcesses() {
    std::unordered_set<DWORD> potentialProcesses;
    
    for (DWORD pid = 8; pid <= 65536; pid++) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        DWORD lastError = GetLastError();
        
        // 记录所有可以打开的进程或者返回拒绝访问的进程
        if (hProcess != NULL || lastError == ERROR_ACCESS_DENIED) {
            potentialProcesses.insert(pid);
            
            // 显示扫描进度（每4096个PID显示一次）
            if (pid % 4096 == 0) {
                std::cout << "Scanned up to PID: " << pid << " (" 
                          << (pid * 100 / 65536) << "% complete)" << std::endl;
            }
            
            if (hProcess != NULL) {
                CloseHandle(hProcess);
            }
        }
    }
    
    return potentialProcesses;
}

// 使用Toolhelp32 API获取正常进程列表（快速）
std::unordered_set<DWORD> GetNormalProcessList() {
    std::unordered_set<DWORD> processIds;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed. Error: " << GetLastError() << std::endl;
        return processIds;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (!Process32First(hSnapshot, &pe32)) {
        std::cerr << "Process32First failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hSnapshot);
        return processIds;
    }
    
    do {
        processIds.insert(pe32.th32ProcessID);
    } while (Process32Next(hSnapshot, &pe32));
    
    CloseHandle(hSnapshot);
    return processIds;
}

// 分析隐藏进程
void AnalyzeHiddenProcesses(const std::unordered_set<DWORD>& potentialProcesses) {
    // 获取正常进程列表
    std::unordered_set<DWORD> normalProcesses = GetNormalProcessList();
    using namespace std;
    std::cout << "\nTotal processes: " << normalProcesses.size() << endl << endl;
    
    std::unordered_set<DWORD> hiddenProcesses;
    std::unordered_set<DWORD> protectedProcesses;
    
    // 对比两个集合
    for (DWORD pid : potentialProcesses) {
        if (normalProcesses.find(pid) == normalProcesses.end()) {
            // 检查是否能实际打开进程
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid);
            if (hProcess != NULL) {
DWORD dwExitCode = 0;GetExitCodeProcess(hProcess, &dwExitCode);
                if ((dwExitCode == STILL_ACTIVE) && WaitForSingleObject(hProcess, 0) == WAIT_TIMEOUT) {
		CHAR buffer[2048] = { 0 }; DWORD size = 2047;
		QueryFullProcessImageNameA(hProcess, 0, buffer, &size);
                cout << "PID " << pid << ", code " << dwExitCode << ": " << buffer << endl;
                    hiddenProcesses.insert(pid);
                }
                CloseHandle(hProcess);
            } else if (GetLastError() == ERROR_ACCESS_DENIED) {
                protectedProcesses.insert(pid);
            }
        }
    }
    
    // 输出结果
    std::cout << "\nDetection Results:" << std::endl;
    std::cout << "-----------------" << std::endl;
    
    if (hiddenProcesses.empty() && protectedProcesses.empty()) {
        std::cout << "No hidden processes detected." << std::endl;
        return;
    }
    
    if (!hiddenProcesses.empty()) {
        std::cout << "Found " << hiddenProcesses.size() << " hidden process(es):" << std::endl;
        for (DWORD pid : hiddenProcesses) {
            std::cout << "  PID: " << pid << " (visible to OpenProcess but not in Toolhelp32 list)" << std::endl;
        }
    }
    
    if (!protectedProcesses.empty()) {
        std::cout << "\nFound " << protectedProcesses.size() << " protected process(es):" << std::endl;
        for (DWORD pid : protectedProcesses) {
            std::cout << "  PID: " << pid << " (access denied and not in Toolhelp32 list)" << std::endl;
        }
    }
}