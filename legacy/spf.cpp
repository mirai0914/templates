const int MAX = 2e5;
vector<int> spf;

void
spf_init()
{
    spf.resize(MAX + 1);
    spf[1] = 1;
    for (int i = 2; i < MAX; i++)
        if (!spf[i])
            for (int j = i; j < MAX; j += i)
                if (!spf[j])
                    spf[j] = i;
}