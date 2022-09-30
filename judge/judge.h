/**
 * @file judge.h
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief 评测姬运行库
 * @version 2.0.0
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#ifndef _JUDGE_H_
#define _JUDGE_H_
#define __windows__ (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))

// 指定linux环境下系统头文件
#ifdef __linux__
#include<unistd.h>
#include<stdlib.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
// 指定Windows环境下系统头文件 
#elif __windows__
#include<Windows.h>
#include<io.h>
#include<direct.h>
#include<psapi.h>
// 指定其他环境头文件
#else 
#error("We only support Windows & Linux system! Sorry for that your system wasn't supported!")
#endif
#include<jsoncpp/json/json.h>
#include<mysql/mysql.h>
#include<pthread.h>

#include"syslib/log.h"
#include"syslib/syslib.h"
/** 指定Linux环境下头文件 */
#ifdef __linux__
#include"syslib/linux.h"
/** 指定Windows环境下头文件 */
#elif __windows__
#include"syslib/windows.h"
#endif
using namespace std;



/**
 * @brief 评测单个测试点
 * 
 * @param thread_id 线程id
 * @param source_path 源文件名
 * @param fin 文件输入名
 * @param fout 文件输出名
 * @param inpath 程序输入文件
 * @param outpath 程序输出文件
 * @param execcmd 程序
 * @param execname 程序监视名
 * @param spj_execparam SPJ额外运行参数
 * @param tl 时间限制
 * @param ml 空间限制
 * @param full_score 测试点满分
 * @return Json::Value 
 */
Json::Value judge_single(int thread_id, string source_path, string fin, string fout, string inpath, string outpath, 
    string execcmd, string execname, string spj_execparam, int tl, int ml, int full_score) {
    string main_path = "./tmp/tmp" + to_string(thread_id) + "/";
    Json::Value info;

    /** 创建需要的文件 */
    ofstream cfout((main_path + fout).c_str());
    cfout.close();
    cfout.open((main_path + "score.txt").c_str());
    cfout.close();
    cfout.open((main_path + "info.txt").c_str());
    cfout.close();
    string spjcmd = getpath(main_path + "spj.exe")
        + " " + inpath 
        + " " + getpath(main_path + fout)
        + " " + outpath
        + " " + to_string(full_score)
        + " " + getpath(main_path + "score.txt")
        + " " + getpath(main_path + "info.txt")
        + " " + source_path;
        + " " + spj_execparam;
    string spjname = getpath(main_path + "spj.exe");

    /** 建立文件链接 */
    unlinkFile(main_path + fin);
    linkFile(inpath, main_path + fin);

    /** 合成指令并运行程序 */
    int rt, rm;
    if (fin == "stdin") execcmd += "< stdin";
    if (fin == "stdput") execcmd += "< stdout";
    int res = judge_cmd(thread_id, execcmd, execname, tl, ml, rt, rm);
    
    /** 程序运行时错误 */
    if (res % (1 << 4) == JUDGE_STATUS_RE) {
        int recode = res / (1 << 4);
        info["info"] = "Runtime Error | " + analyseReason(recode);
        info["memory"] = rm;
        info["time"] = rt;
        info["score"] = 0;
        info["state"] = judge_status[JUDGE_STATUS_RE];
        return info;
    }
    
    /** 程序异常退出 */
    if (res != JUDGE_STATUS_AC) {
        info["info"] = judge_status[res];
        info["memory"] = rm;
        info["time"] = rt;
        info["score"] = 0;
        info["state"] = judge_status[res];
        return info;
    }

    /** 运行spj */
    int rt2, rm2;
    res = judge_cmd(thread_id, spjcmd, spjname, tl, ml, rt2, rm2);

    /** spj异常退出 */
    if (res != JUDGE_STATUS_AC) {
        info["info"] = "Special Judge: " + judge_status[res];
        info["memory"] = rm;
        info["time"] = rt;
        info["score"] = 0;
        info["state"] = judge_status[JUDGE_STATUS_JF];
        return info;
    }

    /** 获取得分 */
    int score = -1;
    ifstream cfin(getpath(main_path + "score.txt").c_str());
    cfin >> score;
    cfin.close();

    /** 获取评测信息 */
    string resinfo = "";
    resinfo = readFile(getpath(main_path + "info.txt"));

    /** 构造结构体并返回 */
    info["info"] = resinfo;
    info["memory"] = rm;
    info["time"] = rt;
    info["score"] = score == -1 ? 0 : score;
    if (score == -1) info["state"] = judge_status[JUDGE_STATUS_WA];
    else if (score == full_score) info["state"] = judge_status[JUDGE_STATUS_AC];
    else info["state"] = judge_status[JUDGE_STATUS_PC];
    return info;
}

#endif