#pragma once
#include <vector>

/**
 * @brief 组合数与阶乘逆元预处理
 * @details 预处理 O(N)，单次查询 O(1)
 *          要求 MOD 为质数
 */
struct Comb {
    int n = 0;
    long long mod = 998244353;
    std::vector<long long> fac, ifac;

    Comb(int max_val = 0, long long m = 998244353) : mod(m) {
        if (max_val > 0) build(max_val);
    }

    static long long qpow(long long a, long long b, long long m) {
        long long res = 1 % m;
        a %= m;
        while (b > 0) {
            if (b & 1) res = (__int128_t)res * a % m;
            a = (__int128_t)a * a % m;
            b >>= 1;
        }
        return res;
    }

    long long inv(long long x) const {
        return qpow(x, mod - 2, mod);
    }

    void build(int max_val) {
        n = max_val;
        fac.assign(n + 1, 1);
        ifac.assign(n + 1, 1);

        for (int i = 1; i <= n; i++) {
            fac[i] = (__int128_t)fac[i - 1] * i % mod;
        }
        ifac[n] = inv(fac[n]);
        for (int i = n; i >= 1; i--) {
            ifac[i - 1] = (__int128_t)ifac[i] * i % mod;
        }
    }

    // 组合数 C(n, k)
    long long C(int a, int b) const {
        if (b < 0 || b > a || a > n) return 0;
        return (__int128_t)fac[a] * ifac[b] % mod * ifac[a - b] % mod;
    }

    // 排列数 A(n, k)
    long long A(int a, int b) const {
        if (b < 0 || b > a || a > n) return 0;
        return (__int128_t)fac[a] * ifac[a - b] % mod;
    }
};
