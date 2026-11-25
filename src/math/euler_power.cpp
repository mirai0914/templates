#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

/**
 * 欧拉降幂 (扩展欧拉定理) 与 O(sqrt(N)) 单点大数欧拉函数
 * 扩展欧拉定理 (无需 gcd(a, m) = 1):
 *   a^b = a^b % m                         (b < phi(m))
 *   a^b = a^( (b % phi(m)) + phi(m) ) % m  (b >= phi(m))
 */
struct EulerPower {
    // 功能: O(sqrt(n)) 试除法求单个大数的欧拉函数 phi(n)
    // 传参: n 目标正整数 (支持至 1e14~1e18)
    ll get_phi(ll n) const {
        ll ans = n;
        for (ll i = 2; i * i <= n; ++i) {
            if (n % i == 0) {
                ans = ans / i * (i - 1);
                while (n % i == 0) n /= i;
            }
        }
        if (n > 1) ans = ans / n * (n - 1);
        return ans;
    }

    // 功能: 快速幂 a^b % m (i128 防溢出, 支持负底数)
    ll ksm(ll a, ll b, ll m) const {
        ll res = 1 % m;
        a = (a % m + m) % m;
        while (b > 0) {
            if (b & 1) res = (i128)res * a % m;
            if (b > 1) a = (i128)a * a % m;
            b >>= 1;
        }
        return res;
    }

    // 功能: 扩展欧拉定理求 a^b % m (b 为大数字符串)
    // 传参: a 底数, b 指数字符串, m 模数
    ll pow_str(ll a, const string& b, ll m) const {
        if (m == 1) return 0;
        ll p = get_phi(m);
        i128 exp = 0;
        bool ge = false;
        for (char c : b) {
            exp = exp * 10 + (c - '0');
            if (exp >= p) {
                ge = true;
                exp %= p;
            }
        }
        if (ge) exp += p;
        return ksm(a, (ll)exp, m);
    }

    // 功能: 扩展欧拉定理求 a^b % m (b 为数值类型)
    ll pow_num(ll a, ll b, ll m) const {
        if (m == 1) return 0;
        ll p = get_phi(m);
        if (b >= p) b = (b % p) + p;
        return ksm(a, b, m);
    }

    // --- 可选扩展: 幂塔递归降幂 (计算 a[l] ^ a[l+1] ^ ... ^ a[r] % m) ---
    ll ksm_ge(ll a, ll b, ll m) const {
        ll res = 1;
        bool flag = (1 >= m);
        if (res >= m) res %= m;
        if (a >= m) { flag = true; a %= m; }
        while (b > 0) {
            if (b & 1) {
                i128 t = (i128)res * a;
                if (t >= m) flag = true;
                res = t % m;
            }
            if (b > 1) {
                i128 t = (i128)a * a;
                if (t >= m) flag = true;
                a = t % m;
            }
            b >>= 1;
        }
        return res + (flag ? m : 0);
    }

    // 功能: 递归计算幂塔有效降幂值 (对 m 取模即得最终答案)
    ll power_tower(const vector<ll>& a, int l, int r, ll m) const {
        if (l > r || m == 1) return 1 >= m ? 1 % m + m : 1;
        if (l == r) return a[l] >= m ? a[l] % m + m : a[l];
        ll p = get_phi(m);
        ll exp = power_tower(a, l + 1, r, p);
        return ksm_ge(a[l], exp, m);
    }

    // 一行小封装: 幂塔求模
    ll query_tower(const vector<ll>& a, int l, int r, ll m) const {
        return power_tower(a, l, r, m) % m;
    }
};

/*
使用示范:
    EulerPower ep;
    ll phi_val = ep.get_phi(1000000007LL);
    ll ans1 = ep.pow_str(2, "1000000000000000000000000000000", 1000000007);
    ll ans2 = ep.query_tower({2, 3, 2}, 0, 2, 1000); // 512
*/
