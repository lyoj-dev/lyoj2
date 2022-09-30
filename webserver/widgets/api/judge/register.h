auto api_judge_register = [](client_conn conn, http_request request, param argv){
    string name = urldecode(argv[0]);
    Judge info = registerJudge(name);
    if (global_errno) api_error_putRequest(conn, global_errno);
    api_putRequest(conn, 200, toJsonObject(info));
};