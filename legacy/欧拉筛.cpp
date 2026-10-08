const int MAXN = 200005;
vector<int> prm;
bool isp[MAXN];
void 
eulerinit()
{
    for (int i = 2; i < MAXN; i++)
    {
        if (!isp[i])
            prm.push_back(i);
        for (int p : prm)
        {
            if (i * p >= MAXN) break;
            isp[i * p] = true;
            if (i % p == 0) break;
        }
    }
}