struct User {
    int uid;
    string name;
    string passwd;
    string email;
    string signature;
    string introduction;
    string header;
    string space_header;
    vector<SimpleProblem> problem_accepted;
    struct Contest {
        SimpleContest info;
        int rank;
        int diff;
    };
    vector<Contest> contest_info;
    int submit_time;
    int rp;
    int rptime;
    int permission;
};