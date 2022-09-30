struct SimpleProblem {
    int pid;
    time_t create_time;
    string title;
    int diff_code;
    bool hidden;
};

struct SimpleContest {
    int cid;
    int title;
    int regnum;
    int probnum;
    bool rated;
    int type;
};

struct SimpleStatus {
    int sid;
    int pid;
    int uid;
    int cid;
    time_t submit_time;
    int lang;
    string result;
    string status;
    int score;
};

struct SimpleUser {
    int uid;
    string name;
    int permission;
    int probacnum;
    int submit_time;
    int rp;
    int rptime;
};