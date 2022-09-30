/**
 * @file syslib.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief Windows与Linux公用函数
 * @version 2.0.0
 * @date 2022-09-17
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#ifndef _SYSLIB_H_
#define _SYSlIB_H_

/**
 * @brief 获取毫秒级时间戳
 * 
 * @return time_t 
 */
time_t clock2() {
	return chrono::duration_cast<chrono::milliseconds>
	(chrono::system_clock::now().time_since_epoch()).count();
}

/**
 * @brief 判断文件是否存在
 * 
 * @param path 文件路径
 * @return true 
 * @return false 
 */
bool isFileExist(string path) {
    ifstream fin(path.c_str());
    return fin.is_open();
}

/**
 * @brief 判断文件夹是否存在
 * 
 * @param path 文件夹路径
 * @return true 
 * @return false 
 */
bool isFolderExist(string path) {
    #ifdef __linux__
        return access(path.c_str(), 0) != -1;
    #elif __windows__
        return _access(path.c_str(), 0) != -1;
    #endif
}

/**
 * @brief 创建文件夹
 * 
 * @param path 文件夹路径
 * @return true 
 * @return false 
 */
bool createDirectory(string path) {
    #ifdef __linux__
        return mkdir(path.c_str(), 0777) != -1;
    #elif __windows__
        return _mkdir(path.c_str()) != -1;
    #endif
}

/**
 * @brief 复制文件
 * 
 * @param from 源文件
 * @param to 目标文件
 * @return true 
 * @return false 
 */
bool copyFile(string from, string to) {
    #ifdef __linux__
    ifstream fin(from.c_str(), ios::binary);
    ofstream fout(to.c_str(), ios::binary);
    if (!fin.is_open() || !fout.is_open()) return false;
    fin.seekg(0, ios::end);
    int len = fin.tellg();
    fin.seekg(0, ios::beg);
    char c[len];
    fin.read(c, len); fout.write(c, len);
    fin.close(); fout.close();
    chmod(to.c_str(), 0777);
    return true; 
    #elif __windows__
    return CopyFile(from.c_str(), to.c_str(), false);
    #endif
}

/**
 * @brief 创建软链接
 * 
 * @param from 链接源文件
 * @param to 链接目标
 * @return true 
 * @return false 
 */
bool linkFile(string from, string to) {
    #ifdef __linux__
    return symlink(from.c_str(), to.c_str()) == 0;
    #elif __windows__
    return CreateSymbolicLinkA(from.c_str(), to.c_str(), 0);
    #endif
}

/**
 * @brief 删除软链接
 * 
 * @param source 链接目标
 * @return true 
 * @return false 
 */
bool unlinkFile(string source) {
    #ifdef __linux__
    return unlink(source.c_str()) == 0;
    #elif __windows__
    return DeleteFile(source.c_str());
    #endif
}

/**
 * @brief 字符串批量替换
 * 
 * @param from 查找
 * @param to 替换为
 * @param source 源字符串
 * @return string
 */
string str_replace(string from, string to, string source) {
    string result = source;
	int st = 0, wh = result.find(from.c_str(), st);
	while (wh != string::npos) {
		result.replace(wh, from.size(), to.c_str());
		st = wh + to.size();
		wh = result.find(from.c_str(), st);
	} 
    return result;
}

/**
 * @brief 读取文件
 * 
 * @param path 文件目录
 * @return string 
 */
string readFile(string path) {
    ifstream t;
    t.open(path.c_str());
    if (!t.is_open()) return "";
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

/**
 * @brief 加密JSON
 * 
 * @param val JSON数据结构体
 * @return string 
 */
string json_encode(Json::Value val) {
    Json::FastWriter writer;
    return writer.write(val);
}

/**
 * @brief 带格式加密结构体
 * 
 * @param val JSON数据结构体
 * @return string 
 */
string json_pretty_encode(Json::Value val) {
    Json::StreamWriterBuilder jswBuilder;
    jswBuilder["emitUTF8"] = true;
    jswBuilder["indentation"] = "  ";
    jswBuilder["enableYAMLCompatibility"] = true;
    unique_ptr<Json::StreamWriter> writer(jswBuilder.newStreamWriter());
    ostringstream buffer;
    writer->write(val, &buffer);
    return buffer.str();
}

/**
 * @brief 解密JSON
 * 
 * @param json json字符串
 * @return Json::Value 
 */
Json::Value json_decode(string json) {
    Json::Reader reader;
    Json::Value val;
    reader.parse(json, val);
    return val;
}

/**
 * @brief 获取运行时错误原因
 * 
 * @param reason 程序退出信号
 * @return string 
 */
string analyseReason(int reason) {
	switch (reason) {
		case 1: return "SIGHUP";break;
		case 2: return "SIGINT";break;
		case 3: return "SIGQUIT";break;
		case 4: return "SIGILL";break;
		case 5: return "SIGTRAP";break;
		case 6: return "SIGABRT";break;
		case 7: return "SIGBUS";break;
		case 8: return "SIGFPE";break;
		case 9: return "SIGKILL";break;
		case 10: return "SIGUSR1";break;
		case 11: return "SIGSEGV";break;
		case 12: return "SIGUSR2";break;
		case 13: return "SIGPIPE";break;
		case 14: return "SIGALRM";break;
		case 15: return "SIGTERM";break;
		case 16: return "SIGSTKFLT";break;
		case 17: return "SIGCHLD";break;
		case 18: return "SIGCONT";break;
		case 19: return "SIGSTOP";break;
		case 20: return "SIGTSTP";break;
		case 21: return "SIGTTIN";break;
		case 22: return "SIGTTOU";break;
		case 23: return "SIGURG";break;
		case 24: return "SIGXCPU";break;
		case 25: return "SIGXFSZ";break;
		case 26: return "SIGVTALRM";break;
		case 27: return "SIGPROF";break;
		case 28: return "SIGWINCH";break;
		case 29: return "SIGIO";break;
		case 30: return "SIGPWR";break;
		case 31: return "SIGSYS";break;
		case 34: return "SIGRTMIN";break;
		case 35: return "SIGRTMIN+1";break;
		case 36: return "SIGRTMIN+2";break;
		case 37: return "SIGRTMIN+3";break;
		case 38: return "SIGRTMIN+4";break;
		case 39: return "SIGRTMIN+5";break;
		case 40: return "SIGRTMIN+6";break;
		case 41: return "SIGRTMIN+7";break;
		case 42: return "SIGRTMIN+8";break;
		case 43: return "SIGRTMIN+9";break;
		case 44: return "SIGRTMIN+10";break;
		case 45: return "SIGRTMIN+11";break;
		case 46: return "SIGRTMIN+12";break;
		case 47: return "SIGRTMIN+13";break;
		case 48: return "SIGRTMIN+14";break;
		case 49: return "SIGRTMIN+15";break;
		case 50: return "SIGRTMAX-14";break;
		case 51: return "SIGRTMAX-13";break;
		case 52: return "SIGRTMAX-12";break;
		case 53: return "SIGRTMAX-11";break;
		case 54: return "SIGRTMAX-10";break;
		case 55: return "SIGRTMAX-9";break;
		case 56: return "SIGRTMAX-8";break;
		case 57: return "SIGRTMAX-7";break;
		case 58: return "SIGRTMAX-6";break;
		case 59: return "SIGRTMAX-5";break;
		case 60: return "SIGRTMAX-4";break;
		case 61: return "SIGRTMAX-3";break;
		case 62: return "SIGRTMAX-2";break;
		case 63: return "SIGRTMAX-1";break;
		case 64: return "SIGRTMAX";break;
		default: return "Unknown Error";break;
	}
	return "Unknown Error";
}

#endif