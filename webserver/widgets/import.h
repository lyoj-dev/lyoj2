#include"variables.h"

#include"db/mysqli.h"
#include"../libraries/import.h"
#include"api/libraries.h"

#include"api/main.h"
#include"api/problem/data.h"
#include"api/problem/info.h"
#include"api/problem/list.h"
#include"api/judge/register.h"
#include"api/judge/token.h"
#include"api/judge/heartbeat.h"
#include"api/judge/info.h"

#include"gui/index.h"

void importRoute() {
    app.addRoute("/api", api_main);
    app.addRoute("/api/problem", api_problem_list);
    app.addRoute("/api/problem/%d", [](client_conn conn, http_request request, param argv){
        if (request.method == "GET") api_problem_info(conn, request, argv);
    });
    app.addRoute("/api/problem/%d/data/%s", api_problem_data);
    app.addRoute("/api/judge/%s", [](client_conn conn, http_request request, param argv){
        if (request.method == "GET") api_judge_info(conn, request, argv);
        else if (request.method == "POST") api_judge_register(conn, request, argv);
        else if (request.method == "PATCH") api_judge_heartbeat(conn, request, argv);
    });
    app.addRoute("/api/judge/%s/token", api_judge_token);

    app.addRoute("/", gui_index);
    app.addRoute("/index", gui_index);
}