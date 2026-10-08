ll fac[MAXN], ifac[MAXN];

ll ksm(ll a, ll b)
{
    ll res = 1;
    while (b)
    {
        if (b & 1)
            res = res * a % MOD;
        a = a * a % MOD;
        b >>= 1;
    }
    return res;
}

void init_c(int n)
{
    fac[0] = 1;
    for (int i = 1; i <= n; i++)
        fac[i] = fac[i - 1] * i % MOD;

    ifac[n] = ksm(fac[n], MOD - 2);

    for (int i = n; i >= 1; i--)
        ifac[i - 1] = ifac[i] * i % MOD;
}

ll C(int n, int k)
{
    if (k < 0 || k > n)
        return 0;
    return fac[n] * ifac[k] % MOD * ifac[n - k] % MOD;
}