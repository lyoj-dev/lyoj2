struct Problem {
    Problem(){};
    int pid = 0;
    time_t create_time = 0;
    string title = "";
    string background = "";
    string description = "";
    string input_desc = "";
    string output_desc = "";
    string input_filename = "";
    string output_filename = "";
    struct Testcase {
        string input = "";
        string output = "";
    };
    vector<Testcase> simple;
    string hint = "";
    int diff_code = 0;
    bool hidden = 0;
};