#include<bits/stdc++.h>
#include<jsoncpp/json/json.h>
#include<mysql/mysql.h>
#include"httpd.h"
#include"syslib/import.h"
#include"widgets/import.h"
using namespace std;
int main(int argc, char** argv) {
    init_error();
    importRoute();
    app.addRoute("/test", [](client_conn conn, http_request request, param argv){
        argvar $_GET = getParam(request);
        argvar $_POST = postParam(request);
        argvar $_COOKIE = cookieParam(request);
        stringstream buffer;
        for (auto it : $_GET) buffer << it.first << " " << it.second << endl;
        for (auto it : $_POST) buffer << it.first << " " << it.second << endl;
        for (auto it : $_COOKIE) buffer << it.first << " " << it.second << endl;
        for (auto it : request.argv) buffer << it.first << " " << it.second << endl;
        cout << buffer.str() << endl;
        putRequest(conn, 200, __default_response);
        send(conn, buffer.str());
        exitRequest(conn);
    });
    app.setopt(HTTP_LISTEN_HOST, "0.0.0.0");
    app.setopt(HTTP_LISTEN_PORT, 8888);
    app.setopt(HTTP_MULTI_THREAD, 32);
    app.setopt(LOG_TARGET_TYPE, LOG_TARGET_CONSOLE | LOG_TARGET_FILE);
    // app.setopt(OPEN_DEBUG, true);
    app.run();
}