/**
 * @brief 获取题目信息
 * 
 * @param lpid 题目id左界
 * @param rpid 题目id右界
 * @param Problem 题目信息结果
 * @return vector<Problem>
 */
vector<Problem> getProblemInfo(int lpid, int rpid) {
    Mysqli connector;

    /** 查询题目信息 */
    vector<argvar> result = connector.query("SELECT * FROM problem WHERE pid >= " + to_string(lpid) + " AND pid <= " + to_string(rpid));

    vector<Problem> res;
    for (int i = 0; i < result.size(); i++) {
        
        /** 构造Problem结构体 */
        argvar raw = result[0];
        Problem tmp;
        tmp.pid = atoi(raw["pid"].c_str());
        tmp.create_time = atoi(raw["createtime"].c_str());
        tmp.title = raw["title"];
        tmp.background = raw["background"];
        tmp.description = raw["description"];
        tmp.input_desc = raw["input_desc"];
        tmp.output_desc = raw["output_desc"];
        tmp.input_filename = raw["input_filename"];
        tmp.output_filename = raw["output_filename"];
        Json::Value tc = json_decode(raw["testcase"]);
        for (int i = 0; i < tc.size(); i++) {
            Problem::Testcase t;
            t.input = tc[i]["input"].asString();
            t.output = tc[i]["output"].asString();
            tmp.simple.push_back(t);
        }
        tmp.hint = raw["hint"];
        tmp.diff_code = atoi(raw["difficulty"].c_str());
        tmp.hidden = atoi(raw["hidden"].c_str());
    }
    return res;
}

/**
 * @brief 简化题目信息
 * 
 * @param info 题目详细信息
 * @return SimpleProblem 
 */
SimpleProblem toSimpleVersion(Problem info) {
    SimpleProblem res;
    res.pid = info.pid;
    res.create_time = info.create_time;
    res.title = info.title;
    res.diff_code = info.diff_code;
    res.hidden = info.hidden;
    return res;
}

/**
 * @brief 将题目信息转换为Json::Value
 * 
 * @param info 题目信息
 * @return Json::Value 
 */
Json::Value toJsonObject(Problem info) {
    Json::Value res;
    res["pid"] = info.pid;
    res["create_time"] = (long long)info.create_time;
    res["title"] = info.title;
    res["background"] = info.background;
    res["description"] = info.description;
    res["input_desc"] = info.input_desc;
    res["output_desc"] = info.output_desc;
    res["input_filename"] = info.input_filename;
    res["output_filename"] = info.output_filename;
    Json::Value testcase;
    for (int i = 0; i < info.simple.size(); i++) {
        Json::Value tmp;
        tmp["input"] = info.simple[i].input;
        tmp["output"] = info.simple[i].output;
        testcase.append(tmp);
    }
    res["testcase"] = testcase;
    res["hint"] = info.hint;
    res["difficulty"] = info.diff_code;
    res["hidden"] = info.hidden;
    return res;
}

/**
 * @brief 将简易题目信息转换为Json::Value
 * 
 * @param info 简易题目信息
 * @return Json::Value 
 */
Json::Value toJsonObject(SimpleProblem info) {
    Json::Value res;
    res["pid"] = info.pid;
    res["create_time"] = (long long)info.create_time;
    res["title"] = info.title;
    res["difficulty"] = info.diff_code;
    res["hidden"] = info.hidden;
    return res;
}