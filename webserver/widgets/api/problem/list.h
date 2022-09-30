auto api_problem_list = [](client_conn conn, http_request request, param argv){
    if (request.argv.find("l") == request.argv.end()) api_error_putRequest(conn, PARAM_NOT_FOUND);
    if (request.argv.find("r") == request.argv.end()) api_error_putRequest(conn, PARAM_NOT_FOUND);
    
};