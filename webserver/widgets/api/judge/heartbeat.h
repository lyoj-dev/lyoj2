auto api_judge_heartbeat = [](client_conn conn, http_request request, param argv){
    auto $_POST = postParam(request);
    string name = argv[0];
    if ($_POST.find("token") == $_POST.end()) api_error_putRequest(conn, PARAM_NOT_FOUND);
    auto info = updateHeartbeat(name, $_POST["token"]);
    if (global_errno) api_error_putRequest(conn, global_errno);
    api_putRequest(conn, 200, toJsonObject(info));
};