void api_putRequest(client_conn conn, int code, Json::Value val) {
    Json::Value tmp;
    tmp["code"] = 0;
    tmp["message"] = "";
    tmp["data"] = val;
    tmp["ttl"] = 1;
    putRequest(conn, 200, merge(__default_response, mime(".json")));
    send(conn, json_encode(tmp));
    exitRequest(conn);
}

void api_pretty_putRequest(client_conn conn, int code, Json::Value val) {
    Json::Value tmp;
    tmp["code"] = 0;
    tmp["message"] = "";
    tmp["data"] = val;
    tmp["ttl"] = 1;
    putRequest(conn, 200, merge(__default_response, mime(".json")));
    send(conn, json_pretty_encode(tmp));
    exitRequest(conn);
}

void api_error_putRequest(client_conn conn, int err) {
    putRequest(conn, 200, merge(__default_response, mime(".json")));
    Json::Value tmp;
    const int len = 1024 * 1024;
    tmp["code"] = err;
    tmp["message"] = err_msg[err];
    tmp["ttl"] = 1;
    send(conn, json_encode(tmp));
    exitRequest(conn);
}