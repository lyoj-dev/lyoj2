/**
 * @file linux.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief Linux库函数调用
 * @version 2.0.0
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#ifndef _LINUX_H_
#define _LINUX_H_
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
    if (realpath(path.c_str(), res)) return res;
    else return "-";
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
    stream = popen(cmd.c_str(), "r");
    if (stream == NULL) {
        writeLog(LOG_LEVEL_WARNING, "Failed to open pipe stream!");
        return -1;
    }
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
	res = string(buf);
    int ret = pclose(stream);
    // if (ret == -1) writeLog(LOG_LEVEL_WARNING, "Failed to close pipe stream!(May caused by signal handler)");
	return ret;
}

/**
 * @brief 切换工作路径
 * 
 * @param path 工作路径
 */
void _chdir(string path) {
	int retc = chdir(path.c_str());
    if (retc == -1) writeLog(LOG_LEVEL_WARNING, "Failed to change work path!");
}

/**
 * 
 * @brief 获取程序运行内存
 * 
 * @param pid 程序pid
 * @return int
 */
int get_proc_mem(int pid){
	char* line_buff;

    /** 读取程序状态文件 */
	ifstream fin(string("/proc/" + to_string(pid) + "/status").c_str());
	if (!fin.is_open()) return 0;
	stringstream tmp; 
	tmp << fin.rdbuf();
	string buffer = tmp.str();
	fin.close(); 

    /** 解析程序状态文件 */
	char name[1024]; int vmrss;
	line_buff = strtok(const_cast<char*>(buffer.c_str()),"\n");
	while(line_buff != NULL){
		sscanf(line_buff, "%s", name);

        /** 判断是否为僵尸进程 */
		if ((string)name == "State:") {
			char state[1024] = "";
			sscanf(line_buff, "%s %s", name, state);
			if (string(state) == "Z") return 0;
		}

        /** 读取内存信息 */
		if ((string)name == "VmRSS:") {
			sscanf(line_buff, "%s %d", name, &vmrss);
			return vmrss;
		}

        line_buff = strtok(NULL, "\n");
	} 
    return 0;
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
    "Skipped",
    "System Error",
    "No Test Data",
    "Judgement Failed"
};

map<int, int> thread_mapping;

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
    string tmp_path = "./tmp/tmp" + to_string(thread_id) + "/";

    /** 构造运行脚本 */
    ofstream fout((tmp_path + "run.sh").c_str());
    fout << "cd " << tmp_path << endl;
    fout << "ulimit -s unlimited" << endl;
    fout << cmd << endl;
    fout << "echo $? > code" << endl;
    fout << "cd ../../" << endl;
    fout.close();
    bool re_state = false, re_reason = 0;

    /** 创建子进程 */
    int exec = fork();
    thread_mapping[pthread_self()] = exec;
    signal(SIGCHLD, [](int sig){
        if (sig == SIGCHLD) {
            int status = 0;
            if (thread_mapping[pthread_self()] != 0)
                pid_t pid = waitpid(thread_mapping[pthread_self()], &status, WNOHANG);
        }
    });

    if (exec < 0) {
        writeLog(LOG_LEVEL_INFO, "Failed to create child process, stop to run program!");
        signal(SIGCHLD, SIG_DFL);
        return JUDGE_STATUS_SE;
    } else if (exec == 0) {
        const int len = 1024;
        char* argv[len] = {NULL};
        string run_sh_path = tmp_path + "run.sh";

        /** 构建运行参数并运行程序 */
        argv[0] = const_cast<char*>("bash");
        argv[1] = const_cast<char*>(run_sh_path.c_str());
        argv[2] = NULL;

        execvp("bash", argv);
    } else {
        /** 开始计时 */
        time_t st = clock2();
        rm = 1520;

        /** 获取程序运行id */
        writeLog(LOG_LEVEL_DEBUG, "Successfully create child process, pid: " + to_string(exec));
        int pid = -1;
        while (pid == -1 && kill(exec, 0) == 0) {
            string tmp;
            system2(("ps -ef | grep \"" + name + "\" | grep -v grep | awk '{print $2}' | head -n1").c_str(), tmp);
            if (tmp != "") pid = atoi(tmp.c_str());
        }
        if (pid == -1) {
            writeLog(LOG_LEVEL_WARNING, "Program exited too fast!");
            rt = clock2() - st;
        }
        else writeLog(LOG_LEVEL_DEBUG, "Successfully get pid: " + to_string(pid));

        /** 循环检测程序运行状态 */
        while(1) {
            /** 正常退出或运行时错误 */
            if (kill(exec, 0)) {
                ifstream fin(tmp_path + "code");
                int re_code; 
                fin >> re_code;
                if (re_code) re_code -= 128;
                rt = clock2() - st;
                writeLog(LOG_LEVEL_INFO, "Program exited with return value " + to_string(re_code) + ".");
                writeLog(LOG_LEVEL_INFO, "Time Usage: " + to_string(rt) + "ms, Memory Usage: " + to_string(rm) + "kb");
                fin.close();
                signal(SIGCHLD, SIG_DFL);
                if (re_code) return JUDGE_STATUS_RE + (re_code << 4);
                else return JUDGE_STATUS_AC;
            }
            rt = clock2() - st, rm = max(rm, get_proc_mem(pid));

            /** 超过时限或空限 */
            if (rt > tl || rm > ml) {
                writeLog(LOG_LEVEL_INFO, "Program exited caused by " + string(rt > tl ? "Time Limited Exceeded" : "Memory Limited Exceeded") + ".");
                writeLog(LOG_LEVEL_INFO, "Time Usage: " + to_string(rt) + "ms, Memory Usage: " + to_string(rm) + "kb");
                string result = "";
                if (system2(("kill " + to_string(pid) + " 2>&1"), result) && result.find("No such process") != string::npos) writeLog(LOG_LEVEL_WARNING, "Failed to terminate program!");
                if (system2(("kill " + to_string(exec) + " 2>&1"), result) && result.find("No such process") != string::npos) writeLog(LOG_LEVEL_WARNING, "Failed to terminate child process!");
                signal(SIGCHLD, SIG_DFL);
                return rt > tl ? JUDGE_STATUS_TLE : JUDGE_STATUS_MLE;
            }
        }
    }
    signal(SIGCHLD, SIG_DFL);
    return JUDGE_STATUS_SE;
}

#endif