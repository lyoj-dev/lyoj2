struct Contest {
    Contest(){};
    int cid;
    string title;
    string description;
    vector<SimpleProblem> problem;
    vector<vector<int> > ranklist;
    vector<SimpleStatus> status;
    int regnum;
    bool rated;
    int type;
};