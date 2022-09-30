/**
 * @brief 获取评测机信息
 * 
 * @param name 评测机名称
 * @return Judge 
 */
Judge getJudgeInfo(string name) {
    Mysqli connector;

    /** 构造sql并查询 */
    name = quoteEncode(name);
    string sql = "SELECT * FROM judger WHERE name=\"" + name + "\"";
    auto res = connector.query(sql);
    
    /** 无结果 */
    if (res.size() == 0) return global_errno = JUDGE_NOT_FOUND, Judge();

    /** 构造结构体 */
    Judge ret;
    ret.name = res[0]["name"];
    ret.verified = atoi(res[0]["verified"].c_str());
    ret.heartbeat = atoi(res[0]["heartbeat"].c_str());
    ret.active = time(NULL) - atoi(res[0]["heartbeat"].c_str()) <= 10;
    return ret;
}

/**
 * @brief 注册评测机
 * 
 * @param name 评测机名称
 * @return Judge
 */
Judge registerJudge(string name) {
    Mysqli connector;

    /** 构建sql并执行 */
    name = quoteEncode(name);
    string sql = "INSERT INTO judger (name, token, verified, heartbeat)";
    sql += " VALUES (\"" + name + "\", \"\", 0, 0)";
    connector.execute(sql);

    /** 获取评测机信息并返回 */
    return getJudgeInfo(name);
}

/**
 * @brief 核验评测机token
 * 
 * @param name 评测机名称
 * @param token 评测机token
 * @return true 
 * @return false 
 */
bool checkToken(string name, string token) {
    Mysqli connector;
    
    /** 构建sql并查询 */
    name = quoteEncode(name);
    token = quoteEncode(token);
    string sql = "SELECT name FROM judger WHERE name = \"" + name + "\" AND token = \"" + token + "\"";
    auto res = connector.query(sql);

    return res.size() == 1;
}

/**
 * @brief 上传心跳
 * 
 * @param name 评测机名称
 * @param token 评测机token
 * @return Judge 
 */
Judge updateHeartbeat(string name, string token) {
    Mysqli connector;

    /** 核验token */
    if (!checkToken(name, token)) {
        global_errno = JUDGE_INVALID_TOKEN;
        return Judge();
    }

    /** 构建sql并执行 */
    name = quoteEncode(name);
    string sql = "UPDATE judger SET heartbeat = " + to_string(time(NULL)) + 
                 " WHERE name = \"" + name + "\"";
    connector.execute(sql);

    return getJudgeInfo(name);
}

/**
 * @brief 转换为Json结构体
 * 
 * @param info 评测机信息
 * @return Json::Value 
 */
Json::Value toJsonObject(Judge info) {
    Json::Value ret;
    ret["name"] = info.name;
    ret["verified"] = info.verified;
    ret["heartbeat"] = info.heartbeat;
    ret["active"] = info.active;
    return ret;
}