auto gui_index = [](client_conn conn, http_request request, param argv){
    putRequest(conn, 200, __default_response);
    stringstream buffer;
    buffer << "Welcome to LittleYang OnlineJudge v2!" << _endl;
    buffer << _endl;
    buffer << "This is another version of lyoj!" << _endl;
    buffer << "But the Interactive Interface is empty now!" << _endl;
    buffer << "If you want to submit problem, Please go to <a href=\"http://10.115.80.253\">http://10.115.80.253</a>!" << _endl;
    buffer << "Thanks for your support!" << _endl;
    send(conn, buffer.str());
    exitRequest(conn);
};