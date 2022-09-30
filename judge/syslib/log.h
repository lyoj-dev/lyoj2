/**
 * @file log.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief 标准日志输出库
 * @version 2.0.0
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#ifndef _LOG_H_
#define _LOG_H_

#ifndef __windows__
#define __windows__ (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#endif

#ifdef __linux__
#include<unistd.h>
#include<syscall.h>
#elif __windows__
#include<Windows.h>
#endif
#include<pthread.h>
using namespace std;

enum LOG_LEVEL {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO
};

string log_file_target = "./log.txt";
int target_id = 0;
ofstream log_fout;
pthread_mutex_t log_lock;

enum LOG_TARGET {
    LOG_TARGER_NONE = 0,
    LOG_TARGET_CONSOLE = 1,
    LOG_TARGET_FILE = 2
};

/**
 * @brief 初始化日志系统
 * 
 * @param log_target 日志输出目标
 */
void init_log(int log_target) {
    target_id = log_target;
    if (target_id & LOG_TARGET_FILE) log_fout.open(log_file_target.c_str(), ios::app);
}

#define writeLog(loglevel, dat) __writeLog(loglevel, __FILE__, __LINE__, dat)

/**
 * @brief 写入日志
 * 
 * @param loglevel 日志等级
 * @param fileName 文件名
 * @param lineNumber 行号
 * @param dat 数据
 */
void __writeLog(LOG_LEVEL loglevel, string fileName, int lineNumber, string dat) {
    stringstream buffer;
	time_t timep = time(&timep);
    char tmp[1024] = "";
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));

    string level_str = "";
    switch(loglevel) {
        case LOG_LEVEL_NONE: level_str = "NONE"; break;
        case LOG_LEVEL_INFO: level_str = "INFO"; break;
        case LOG_LEVEL_WARNING: level_str = "WARNING"; break;
        case LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        case LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
        default: level_str = "UNKNOWN"; break;		      
    }

    int tid = -1;
    #ifdef __linux__
    tid = syscall(__NR_gettid);
    #elif __windows__
    tid = GetCurrentThreadId();
    #endif

    buffer << "[" << tmp << "] [" << level_str << "] [tid:" << tid << "] [" << fileName << ":" << lineNumber << "] " << dat;
    pthread_mutex_lock(&log_lock);
    if (target_id & LOG_TARGET_CONSOLE) cout << buffer.str() << endl;
    if (target_id & LOG_TARGET_FILE) log_fout << buffer.str() << endl;
    pthread_mutex_unlock(&log_lock);
}

#endif