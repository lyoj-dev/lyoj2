/**
 * @file judge.cpp
 * @author LittleYang0531 (dev@lyoj.ml)
 * @brief 评测姬入口文件
 * @version 2.0.0
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022 LittleYang0531
 * 
 */

#include<bits/stdc++.h>
#include"judge.h"
#include"widgets/import.h"
using namespace std;
pthread_t pt[1024];
pthread_mutex_t pmutex;
int cnt = 0;
Json::Value lang_config, spj_config;

struct status{
    int sid, pid;
    int lang;
    string code;
};
/** 工作队列 */
vector<status> q;

/**
 * @brief 分配线程id
 * 
 * @return int 
 */
int getid() {
    pthread_mutex_lock(&pmutex);
    int res = ++cnt;
    pthread_mutex_unlock(&pmutex);
    return res;
}

/**
 * @brief 分配工作队列
 * 
 * @param res 工作队列
 * @return true 
 * @return false 
 */
bool getstatus(status& res) {
    bool ok = false;
    pthread_mutex_lock(&pmutex);
    if (q.size()) ok = true, res = q.front(), q.erase(q.begin());
    pthread_mutex_unlock(&pmutex);
    return ok;
}

/**
 * @brief 获取源文件后缀名
 * 
 * @param mode 源文件模式
 * @return string 
 */
string getsuf(string mode) {
    if (mode == "cpp" || mode == "c++") return "cpp";
    if (mode == "c") return "c";
    if (mode == "pascal") return "pas";
    if (mode == "java") return "java";
    if (mode == "php") return "php";
    if  (mode == "python") return "py";
    return "";
}

/**
 * @brief 上传评测结果与详细信息
 * 
 * @param connector MySQL连接符
 * @param sid 评测id 
 * @param result 评测详细信息
 * @param status 评测结果
 */
void submit(Mysqli connector, int sid, string result, string status) {
    result = str_replace("\\", "\\\\", result);
    result = str_replace("\"", "\\\"", result);
    connector.execute("UPDATE status SET result=\"" + result + "\" WHERE id=" + to_string(sid));
    status = str_replace("\\", "\\\\", status);
    status = str_replace("\"", "\\\"", status);
    connector.execute("UPDATE status SET status=\"" + status + "\" WHERE id=" + to_string(sid));
}

/**
 * @brief 仅上传评测结果
 * 
 * @param connector MySQL连接符
 * @param sid 评测id
 * @param status 评测结果
 */
void upload(Mysqli connector, int sid, string status) {
    status = str_replace("\\", "\\\\", status);
    status = str_replace("\"", "\\\"", status);
    connector.execute("UPDATE status SET status=\"" + status + "\" WHERE id=" + to_string(sid));
}

/**
 * @brief 工作线程
 * 
 * @param arg 
 * @return void* 
 */
void* Thread(void* arg) {
    int thread_id = getid();
    string main_dir = "./tmp/tmp" + to_string(thread_id) + "/";
    Mysqli connector;
    Json::Value problem_config;
    writeLog(LOG_LEVEL_INFO, "Thread id " + to_string(thread_id) + " is running!");

    /** 循环监听工作队列 */
    while(1) {
        #ifdef __linux__
        usleep(1000);
        #elif __windows__
        Sleep(1);
        #endif

        status s;
        bool ret = getstatus(s);
        if (!ret) continue;
        Json::Value result;

        /** 获取题目配置文件 */
        if (!isFileExist("/etc/judge/problem/" + to_string(s.pid) + "/config.json")) {
            writeLog(LOG_LEVEL_WARNING, "Failed to open problem config, pid: " + to_string(s.pid));
            result["result"] = "System Error";
            submit(connector, s.sid, json_encode(result), "System Error");
            continue;
        }
        string json = readFile("/etc/judge/problem/" + to_string(s.pid) + "/config.json");
        problem_config = json_decode(json);

        /** 必须变量 */
        int compile_type = atoi(lang_config["lang"][s.lang]["type"].asString().c_str());
        string source_path = main_dir + "user." + getsuf(lang_config["lang"][s.lang]["mode"].asString()); 
        string binary_path = main_dir + "main.exe";
        string compile_command = "";
        string running_command = lang_config["lang"][s.lang]["exec_command"].asString();
        if (compile_type != 1) compile_command = lang_config["lang"][s.lang]["command"].asString();

        /** 将代码输出到指定文件 */
        ofstream fout(source_path.c_str());
        if (!fout.is_open()) {
            writeLog(LOG_LEVEL_WARNING, "Failed to write code to the \"" + source_path + "\"");
            result["result"] = "System Error";
            submit(connector, s.sid, json_encode(result), "System Error");
            continue;
        }
        fout << s.code;
        fout.close();
        fout.open(binary_path.c_str());
        fout.close();

        /** 完善编译指令 */
        string real_source_path = getpath(source_path);
        if (real_source_path == "-") {
            writeLog(LOG_LEVEL_WARNING, "Failed to get file path: " + source_path);
            result["result"] = "System Error";
            submit(connector, s.sid, json_encode(result), "System Error");
            continue;
        }
        string real_binary_path = getpath(binary_path);
        if (real_binary_path == "-") {
            writeLog(LOG_LEVEL_WARNING, "Failed to get file path: " + source_path);
            result["result"] = "System Error";
            submit(connector, s.sid, json_encode(result), "System Error");
            continue;
        }
        compile_command = str_replace("$source$", real_source_path, compile_command);
        compile_command = str_replace("$binary$", real_binary_path, compile_command);
        running_command = str_replace("$source$", real_source_path, running_command);
        running_command = str_replace("$binary$", real_binary_path, running_command);
        compile_command = "sudo " + compile_command + " 2>&1";

        /** 编译代码 */
        string compile_info = "";
        string sql = "UPDATE status SET status=\"Compiling...\" WHERE id=" + to_string(s.sid);
        connector.execute(sql);
        if (compile_type != 1) {
            writeLog(LOG_LEVEL_INFO, "Compiling user." + getsuf(lang_config["lang"][s.lang]["mode"].asString()) + "...");
            time_t st = clock2();
            string tmp_compile_info = "";
            int ret = system2(compile_command, tmp_compile_info);
            if (tmp_compile_info != "") compile_info += "In user.code:\r\n" + tmp_compile_info + "\r\n";

            /** 编译失败 */
            if (ret != 0) {
                writeLog(LOG_LEVEL_INFO, "Compile Error after " + to_string(clock2() - st) + "ms!");
                result["compile_info"] = compile_info;
                result["result"] = "Compile Error";
                submit(connector, s.sid, json_encode(result), "Compile Error");
                continue;
            }

            /** 编译成功 */
            writeLog(LOG_LEVEL_INFO, "Compile Success, use " + to_string(clock2() - st) + "ms!");
        }

        /** 编译SPJ */
        int spj_type = atoi(problem_config["spj"]["type"].asString().c_str());
        string spj_binary_path = "";
        if (spj_type <= 0 || spj_type > spj_config.size()) {
            string source_path = "/etc/judge/problem/" + to_string(s.pid) + "/" + problem_config["spj"]["source"].asString();
            string binary_path = "/etc/judge/problem/" + to_string(s.pid) + "/spj.exe";
            string spj_compile_command = "g++ " + source_path + " -o " + binary_path;

            writeLog(LOG_LEVEL_INFO, "Compiling spj.cpp...");
            time_t st = clock2();
            string tmp_compile_info = "";
            int ret = system2(spj_compile_command, tmp_compile_info);
            if (tmp_compile_info != "") compile_info += "In spj.cpp:\r\n" + tmp_compile_info + "\r\n";

            /** 编译失败 */
            if (ret != 0) {
                writeLog(LOG_LEVEL_INFO, "Compile Error after " + to_string(clock2() - st) + "ms!");
                result["compile_info"] = compile_info;
                result["result"] = "Compile Error";
                submit(connector, s.sid, json_encode(result), "Compile Error");
                continue;
            }

            /** 编译成功 */
            writeLog(LOG_LEVEL_INFO, "Compile Success, use " + to_string(clock2() - st) + "ms!");
            spj_binary_path = binary_path;
        } else spj_binary_path = getpath(spj_config[spj_type - 1]["path"].asString());
        result["compile_info"] = compile_info;

        /** 复制文件至临时文件夹 */
        fout.open((main_dir + "spj.exe").c_str()); fout.close();
        if (!copyFile(spj_binary_path, getpath(main_dir + "spj.exe"))) {
            writeLog(LOG_LEVEL_WARNING, "Failed to copy " + spj_binary_path);
            result["result"] = "System Error";
            submit(connector, s.sid, json_encode(result), "System Error");
            continue;
        }
        spj_binary_path = getpath(main_dir + "spj.exe");

        /** 整理Subtask及依赖关系 */
        const int len = 1024 * 16;
        vector<int> subtask[len];
        bool accepted[len];
        for (int i = 0; i < problem_config["data"].size(); i++) 
            subtask[atoi(problem_config["data"][i]["subtask"].asString().c_str())].push_back(i);
        for (int i = 0; i < len; i++) accepted[i] = true;
        int testcase = 0;
        int sumt = 0, maxm = 0;
        Json::Value data;
        result["result"] = judge_status[JUDGE_STATUS_AC];

        /** 正式开始评测 */
        int depend_pt = -1;
        for (int i = 0; i < len; i++) {
            if (subtask[i].size() == 0) continue;
            if (subtask[i].size() && i) depend_pt++;
            bool enter = true;
            for (int j = 0; depend_pt < problem_config["subtask_depend"].size() && 
                            j < i && problem_config["subtask_depend"][depend_pt].size(); j++)
                if (!accepted[atoi(problem_config["subtask_depend"][depend_pt][j].asString().c_str())]) enter = false;
            
            /** 不满足依赖条件 */
            if (!enter) {
                for (int j = 0; j < subtask[i].size(); j++) {
                    testcase++;
                    Json::Value tmp;
                    tmp["info"] = judge_status[JUDGE_STATUS_SK];
                    tmp["memory"] = 0;
                    tmp["time"] = 0;
                    tmp["score"] = 0;
                    tmp["state"] = judge_status[JUDGE_STATUS_SK];
                    data.append(tmp);
                }
                upload(connector, s.sid, "Running on Test " + to_string(testcase) + "...");
                continue;
            }

            /** 满足依赖条件，开始评测 */
            for (int j = 0; j < subtask[i].size(); j++) {
                testcase++;
                upload(connector, s.sid, "Running on Test " + to_string(testcase) + "...");
                int id = subtask[i][j];
                string inpath = "/etc/judge/problem/" + to_string(s.pid) + "/" + problem_config["data"][id]["input"].asString();
                string outpath = "/etc/judge/problem/" + to_string(s.pid) + "/" + problem_config["data"][id]["output"].asString();
                int full_score = atoi(problem_config["data"][id]["score"].asString().c_str());
                int tl = atoi(problem_config["data"][id]["time"].asString().c_str());
                int ml = atoi(problem_config["data"][id]["memory"].asString().c_str());
                Json::Value tmp = judge_single(
                    thread_id, real_source_path, problem_config["input"].asString(), problem_config["output"].asString(),
                    inpath, outpath, running_command, binary_path, problem_config["spj"]["exec_param"].asString(), 
                    tl, ml, full_score);
                
                data.append(tmp);
                sumt += tmp["time"].asInt();
                maxm = max(maxm, tmp["memory"].asInt());

                /** 如果当前结果错误 */
                if (tmp["state"].asString() != judge_status[JUDGE_STATUS_AC]) {
                    if (result["result"].asString() == judge_status[JUDGE_STATUS_AC]) 
                        result["result"] = tmp["state"].asString();
                    if (i) {
                        accepted[i] = false;
                        for (int k = j + 1; k < subtask[i].size(); k++) {
                            testcase++;
                            Json::Value tmp;
                            tmp["info"] = judge_status[JUDGE_STATUS_SK];
                            tmp["memory"] = 0;
                            tmp["time"] = 0;
                            tmp["score"] = 0;
                            tmp["state"] = judge_status[JUDGE_STATUS_SK];
                            data.append(tmp);
                        }
                        upload(connector, s.sid, "Running on Test " + to_string(testcase) + "...");
                        break;
                    }
                }
            }
        }

        /** 上传最终结果 */
        result["time"] = sumt;
        result["memory"] = maxm;
        result["info"] = data;
        submit(connector, s.sid, json_encode(result), result["result"].asString());
    }
}

int main(int argc, char** argv) {
    /** 初始化日志级别 */
    init_log(LOG_TARGET_CONSOLE);

    /** 读取并解析评测姬配置信息 */
    if (!isFileExist("./config/judge.json")) {
        writeLog(LOG_LEVEL_ERROR, "Couldn't find config file in \"./config/judge.json\"");
        return 0;
    }
    if (!isFileExist("./config/lang.json")) {
        writeLog(LOG_LEVEL_ERROR, "Couldn't find config file in \"./config/lang.json\"");
        return 0;
    }
    if (!isFileExist("./config/spj.json")) {
        writeLog(LOG_LEVEL_ERROR, "Couldn't find config file in \"./config/spj.json\"");
        return 0;
    }
    string json = readFile("./config/judge.json");
    Json::Value config = json_decode(json);
    json = readFile("./config/lang.json");
    lang_config = json_decode(json);
    json = readFile("./config/spj.json");
    spj_config = json_decode(json);

    /** 将配置信息写入全局变量 */
    mysqli_host = config["mysql"]["host"].asString();
    mysqli_port = atoi(config["mysql"]["port"].asString().c_str());
    mysqli_user = config["mysql"]["user"].asString();
    mysqli_passwd = config["mysql"]["passwd"].asString();
    mysqli_db = config["mysql"]["database"].asString();

    thread_num = atoi(config["judge"]["thread"].asString().c_str());
    judge_name = config["judge"]["name"].asString();

    /** 校验MySQL/MariaDB连接性 */
    Mysqli connector;
    if (mysql_ping(&connector.mysqld)) return 0;
    
    /** 创建临时文件夹 */
    if (!isFolderExist("./tmp")) {
        if (!createDirectory("./tmp")) {
            writeLog(LOG_LEVEL_ERROR, "Failed to create \"./tmp\" folder!");
            return 0;
        } else writeLog(LOG_LEVEL_INFO, "Successfully create \"./tmp\" folder!");
    } else writeLog(LOG_LEVEL_INFO, "\"./tmp\" is exist, skip to create!");
    for (int i = 1; i <= thread_num; i++) {
        if (!isFolderExist("./tmp/tmp" + to_string(i))) {
            if (!createDirectory("./tmp/tmp" + to_string(i))) {
                writeLog(LOG_LEVEL_ERROR, "Failed to create \"./tmp/tmp" + to_string(i) + "\" folder!");
                return 0;
            } else writeLog(LOG_LEVEL_INFO, "Successfully create \"./tmp/tmp" + to_string(i) + "\" folder!");
        } else writeLog(LOG_LEVEL_INFO, "\"./tmp/tmp" + to_string(i) + "\" is exist, skip to create!");
    }

    /** 创建工作线程 */
    for (int i = 1; i <= thread_num; i++) {
        int ret = pthread_create(&pt[i], NULL, Thread, NULL);
        if (ret != 0) {
            writeLog(LOG_LEVEL_ERROR, "Failed to create thread!");
            return 0;
        }
    }

    /** 开始监听MySQL/MariaDB服务器 */
    while(1) {
        #ifdef __linux__
        usleep(10000);
        #elif __windows__
        Sleep(10);
        #endif
        if (q.size() != 0) continue;
        vector<argvar> res = connector.query("SELECT * FROM status WHERE judged=0 LIMIT 1");
        if (res.size() == 0) continue;

        /** 提取信息 */
        argvar row = res[0];
        int sid = atoi(row["id"].c_str());
        int pid = atoi(row["pid"].c_str());
        int lang = atoi(row["lang"].c_str());
        string code = row["code"];

        /** 更新状态并塞入工作队列 */
        connector.execute("UPDATE status SET judged=1 WHERE id=" + to_string(sid));
        status new_s;
        new_s.pid = pid;
        new_s.sid = sid;
        new_s.lang = lang;
        new_s.code = code;
        q.push_back(new_s);
    }
}