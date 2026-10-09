#pragma once
#include <vector>
#include <algorithm>

/**
 * @brief 快速数论变换 (NTT - Number Theoretic Transform)
 * @details 适用于在模 998244353 等具备原根的特殊质数模数下进行多项式乘法与卷积。
 *          复杂度:
 *          - 多项式乘法 mult: O((N + M) log(N + M))
 *          - 多项式快速幂 poly_pow: O(L log L log k)，L 为限制度数 lim
 *          纯局部状态，多测绝对安全。
 */
struct NTT 
{
    static constexpr int MOD = 998244353;
    const int G = 3;

    long long ksm(long long a, long long b) const {
        long long res = 1;
        a %= MOD;
        for (; b; b >>= 1, a = a * a % MOD) {
            if (b & 1) res = res * a % MOD;
        }
        return res;
    }

    long long inv(long long a) const {
        return ksm(a, MOD - 2);
    }

    void transf(std::vector<int>& a, bool inv_flag) const {
        int n = a.size();
        if (n <= 1) return; // 边界保护：避免 bit - 1 = -1 导致移位 UB

        std::vector<int> rev(n, 0);
        int bit = __builtin_ctz(n);

        for (int i = 0; i < n; ++i) {
            rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (bit - 1));
            if (i < rev[i]) std::swap(a[i], a[rev[i]]);
        }

        for (int len = 2; len <= n; len <<= 1) {
            long long wlen = ksm(G, (MOD - 1) / len);
            if (inv_flag) wlen = inv(wlen);
            int half = len >> 1;

            for (int i = 0; i < n; i += len) {
                long long w = 1;
                for (int j = 0; j < half; ++j) {
                    int u = a[i + j];
                    int v = 1LL * a[i + j + half] * w % MOD;
                    a[i + j] = (u + v >= MOD ? u + v - MOD : u + v);
                    a[i + j + half] = (u - v < 0 ? u - v + MOD : u - v);
                    w = w * wlen % MOD;
                }
            }
        }

        if (inv_flag) {
            long long inv_n = inv(n);
            for (int i = 0; i < n; ++i) {
                a[i] = 1LL * a[i] * inv_n % MOD;
            }
        }
    }

    std::vector<int> mult(std::vector<int> a, std::vector<int> b) const {
        if (a.empty() || b.empty()) return {};
        int sz = a.size() + b.size() - 1;
        int n = 1;
        while (n < sz) n <<= 1;

        a.resize(n, 0);
        b.resize(n, 0);

        transf(a, 0);
        transf(b, 0);
        for (int i = 0; i < n; ++i) {
            a[i] = 1LL * a[i] * b[i] % MOD;
        }
        transf(a, 1);

        a.resize(sz);
        return a;
    }

    std::vector<int> poly_pow(std::vector<int> a, long long k, int lim = -1) const {
        std::vector<int> res = {1};
        while (k > 0) {
            if (k & 1) {
                res = mult(res, a);
                if (lim != -1 && (int)res.size() > lim + 1) res.resize(lim + 1);
            }
            a = mult(a, a);
            if (lim != -1 && (int)a.size() > lim + 1) a.resize(lim + 1);
            k >>= 1;
        }
        return res;
    }
};
