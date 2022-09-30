/**
 * @file windows.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief Windows库函数调用
 * @version 2.0.0
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#ifndef _WINDOWS_H_
#define _WINDOWS_H_
using namespace std;

/**
 * @brief 获取文件绝对路径
 * 
 * @param path 文件相对路径
 * @return string 
 */
string getpath(string path) {
    const int length = 1024 * 1024;
    char res[length];
    memset(res, '\0', sizeof res);
    if (_fullpath(res, path.c_str(), length)) return res;
    else {
        writeLog(LOG_LEVEL_ERROR, "Failed to get file path: " + path);
        exit(3);
    }
}

/**
 * @brief 系统指令运行并获取输出
 * 
 * @param cmd 系统指令
 * @param res 输出结果
 * @return int 
 */
int system2(string cmd, string& res) {
    FILE *stream; 
    const int length = 1024 * 1024;
    char buf[length]; 
    memset(buf, '\0', sizeof(buf));
    stream = _popen(cmd.c_str(), "r");
    if (stream == NULL) {
        writeLog(LOG_LEVEL_WARNING, "Failed to open pipe stream!");
        return -1;
    }
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    res = buf;
    int ret = _pclose(stream);
    if (ret == -1) writeLog(LOG_LEVEL_WARNING, "Failed to close pipe stream!");
	return ret;
}

/**
 * @brief 切换工作路径
 * 
 * @param path 工作路径
 */
void _chdir(string path) {
	int retc = SetCurrentDirectory(path.c_str());
    if (retc == 0) writeLog(LOG_LEVEL_WARNING, "Failed to change work path!");
}

/**
 * @brief 获取程序运行内存
 * 
 * @param hProcess 程序标识符
 * @return int
 */
int get_proc_mem(HANDLE hProcess) {
	PROCESS_MEMORY_COUNTERS mem;
	GetProcessMemoryInfo(hProcess, &mem, sizeof(mem));
	return mem.PeakWorkingSetSize / 1024.0;
}

enum JUDGE_STATUS {
    JUDGE_STATUS_WAITING,   // Waiting...
    JUDGE_STATUS_COMPILING, // Compiling...
    JUSGE_STATUS_RUNNIGN,   // Running...
    JUDGE_STATUS_CE,        // Compile Error
    JUDGE_STATUS_AC,        // Accepted
    JUDGE_STATUS_WA,        // Wrong Answer
    JUDGE_STATUS_TLE,       // Time Limited Exceeded
    JUDGE_STATUS_MLE,       // Memory Limited Exceeded
    JUDGE_STATUS_RE,        // Runtime Error
    JUDGE_STATUS_PC,        // Partially Correct
    JUDGE_STATUS_SK,        // Skipped
    JUDGE_STATUS_SE,        // System Error
    JUDGE_STATUS_NTD,       // No Test Data
    JUDGE_STATUS_JF         // Judgement Failed
};

string judge_status[] = {
    "Waiting...",
    "Compiling...",
    "Running...",
    "Compile Error",
    "Accepted",
    "Wrong Answer",
    "Time Limited Exceeded",
    "Memory Limited Exceeded",
    "Runtime Error",
    "Partially Correct",
    "Skipped"
    "System Error",
    "No Test Data",
    "Judgement Failed"
};

/**
 * @brief 评测程序
 * 
 * @param thread_id 进程id
 * @param cmd 程序运行命令
 * @param name 监控程序名
 * @param tl 程序时间限制
 * @param ml 程序空间限制
 * @param rt 程序实际使用时间
 * @param tm 程序实际使用空间
 * @return int 
 */
int judge_cmd(int thread_id, string cmd, string name, int tl, int ml, int& rt, int& rm) { 
    STARTUPINFO si; PROCESS_INFORMATION pi;

    /** 初始化结构体 */
	ZeroMemory(&si, sizeof(si)); 
    si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

    /** 运行程序 */
    cmd = "./tmp/tmp" + to_string(thread_id) + "/" + cmd;
	char* command = const_cast<char*>(cmd.c_str());
    char* path = const_cast<char*>(("./tmp/tmp" + to_string(thread_id)).c_str());
	bool retc = CreateProcess(NULL, command, NULL, NULL, false, 0, NULL, path, &si, &pi);
    if(!retc) {
	    writeLog(LOG_LEVEL_WARNING, "Failed to execute command!" + to_string(GetLastError()));
        return JUDGE_STATUS_SE;
    } 
 
    /** 循环检测程序运行状态 */
	time_t st = clock2(); 
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, true, pi.dwProcessId);
    int re_code = 0;
	while (1) {
		DWORD exit_code = 0;
		GetExitCodeProcess(hProcess, &exit_code);
		re_code = exit_code;

        /** 正常退出或运行时错误 */
		if (exit_code != STILL_ACTIVE) {
            writeLog(LOG_LEVEL_INFO, "Program exited with return value " + to_string(re_code) + ".");
            writeLog(LOG_LEVEL_INFO, "Time Usage: " + to_string(rt) + "ms, Memory Usage: " + to_string(rm) + "kb");
            if (re_code) return JUDGE_STATUS_RE + (11 << 4);
            else return JUDGE_STATUS_AC;
        } 
        rt = clock2() - st, rm = get_proc_mem(hProcess);

        /** 超过时限或空限 */
		if (rt > tl || rm > ml) {
            writeLog(LOG_LEVEL_INFO, "Program exited caused by " + string(rt > tl ? "Time Limited Exceeded" : "Memory Limited Exceeded") + ".");
            writeLog(LOG_LEVEL_INFO, "Time Usage: " + to_string(rt) + "ms, Memory Usage: " + to_string(rm) + "kb");
			bool ret = TerminateProcess(hProcess, 0);
            if (ret == false) writeLog(LOG_LEVEL_WARNING, "Failed to terminate process pid " + to_string(pi.dwProcessId) + " " + to_string(GetLastError()));
			return rt > tl ? JUDGE_STATUS_TLE : JUDGE_STATUS_MLE;
		}
	}
	CloseHandle(hProcess);
    return JUDGE_STATUS_SE;
}

#endif