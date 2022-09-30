auto api_problem_info = [](client_conn conn, http_request requets, param argv){
    int pid = atoi(argv[0].c_str());
    vector<Problem> info;
    info = getProblemInfo(pid, pid);
    if (info.size() != 1) api_error_putRequest(conn, PROBLEM_ID_NOT_FOUND);
    api_putRequest(conn, 200, toJsonObject(info[0]));
};