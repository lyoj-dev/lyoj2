auto api_judge_info = [](client_conn conn, http_request request, param argv) {
    string name = urldecode(argv[0]);
    Judge info = getJudgeInfo(name);
    if (global_errno) api_error_putRequest(conn, global_errno);
    api_putRequest(conn, 200, toJsonObject(info));
};