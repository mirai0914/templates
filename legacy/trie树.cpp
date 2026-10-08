
struct trie 
{
    static const int MAX = 26;
    vector<array<int, MAX>> tree;
    vector<int> cnt;
    int idx;

    trie() 
    {
        int ttlen = 1e6
        tree.resize(ttlen + 1);
        cnt.resize(ttlen + 1);
        idx = 0;
    }

    void insert(const string &s)
    {
        int p = 0;
        for (char ch : s)
        {
            int c = ch - 'a';
            if (!tree[p][c])
                tree[p][c] = ++idx;
            p = tree[p][c];
        }
        cnt[p]++;
    }

    bool find(const string &s)
    {
        int p = 0;
        for (char ch : s)
        {
            int c = ch - 'a';
            if (!tree[p][c])
                return 0;
            p = tree[p][c];
        }
        return cnt[p] > 0;
    }
};