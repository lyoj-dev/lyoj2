struct Status {
    Status(){};
    int sid;
    SimpleUser user;
    SimpleProblem problem;
    SimpleContest contest;
    time_t submit_time;
    int lang;
    string code;
    string result;
    struct Subtask {
        struct Testinfo {
            string result;
            int time;
            int memory;
            int score;
        };
        vector<Testinfo> testinfo;
        string result;
        int time;
        int memory;
        int score;
    };
    vector<Subtask> subtask;
    int time;
    int memory;
    int score;
    string status;
};