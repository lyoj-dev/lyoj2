auto api_main = [](client_conn conn, http_request request, param argv){
    Json::Value val;
    val["api_info"] = "v2.0.0";
    val["problem_url"] = "/api/problem/{pid}";
    val["problem_addition_url"] = "/api/problem/{pid}/addition/{file}";
    val["status_url"] = "/api/status/{sid}";
    val["contest_url"] = "/api/contest/{cid}";
    val["contest_problem_url"] = "/api/contest/{cid}/problem/{pid}";
    val["contest_problem_addition_val"] = "/api/contest/{cid}/problem/{pid}/addition/{file}";
    val["contest_status_url"] = "/api/contest/{cid}/status/{sid}";
    val["contest_ranklist_url"] = "/api/contest/{cid}/ranklist";
    val["judge_url"] = "/api/judge/{name}";
    val["judge_data_url"] = "/api/judge/problem/{pid}/data/{name}";
    val["judge_config_url"] = "/api/judge/problem/{pid}/config";
    val["user_url"] = "/api/user/{uid}";
    putRequest(conn, 200, merge(__default_response, mime(".json")));
    send(conn, json_pretty_encode(val));
    exitRequest(conn);
};