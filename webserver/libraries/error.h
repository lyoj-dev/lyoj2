#define PARAM_NOT_FOUND 100
#define INVALID_HEADER 101
#define PROBLEM_ID_NOT_FOUND 102
#define JUDGE_NOT_FOUND 103
#define JUDGE_INVALID_TOKEN 104

string err_msg[1024];
void init_error() {
    err_msg[PROBLEM_ID_NOT_FOUND] = "Problem id not found!";
    err_msg[PARAM_NOT_FOUND] = "Parameter not found!";
    err_msg[INVALID_HEADER] = "Invalid header!";
    err_msg[JUDGE_NOT_FOUND] = "Judger not found!";
    err_msg[JUDGE_INVALID_TOKEN] = "Invalid judger token!";
}