const int MAXN = 2e5 + 10;
int lg[MAXN];
vector<vector<int>> st;

void
lg_init()
{
    lg[1] = 0;
    for (int i = 2; i < MAX; i++)
        lg[i] = lg[i >> 1] + 1;
}

void 
st_init(const vector<int> &vec)
{
    int n = vec.size();
    st.assign(n, vector<int>(20));
    for (int i = 0; i < n; i++)
        st[i][0] = vec[i];
    for (int j = 1; j < 20; j++)
        for (int i = 0; i + (1 << j) <= n; i++)
            st[i][j] = max(st[i][j - 1], st[i + (1 << (j - 1))][j - 1]);
}

int 
st_query(int l, int r)
{
    int len = r - l + 1;
    int k = lg[len];
    return max(st[l][k], st[r - (1 << k) + 1][k]);
}
