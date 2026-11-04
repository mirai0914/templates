#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

/**
 * =========================================================================
 * 扩展中国剩余定理 (EXCRT - Extended Chinese Remainder Theorem)
 * =========================================================================
 * 适用场景: 求解同余方程组 x = r_i (mod m_i)，模数 m_i 不保证两两互质。
 * 复杂度: 单次合并 O(log m_i)，总时间复杂度 O(n log(lcm(m_i)))，空间 O(1)。
 * 核心推导备忘 (赛场速查):
 *   已有前 k-1 个方程的特解 ans，模数 M = lcm(m_1 ... m_{k-1})，通解为 ans + t * M。
 *   联立第 k 个方程 ans + t * M = r (mod m) => t * M = r - ans (mod m)。
 *   令 g = exgcd(M, m, t, y)，若 (r - ans) % g != 0 则无解。
 *   否则令 mod_step = m / g，t = t * ((r - ans)/g) % mod_step。
 *   更新 ans += t * M, M *= mod_step, ans %= M。
 * 注意: 中间乘法必须用 i128 防止溢出！
 */
struct EXCRT {
    ll ans = 0; // 当前最小非负解
    ll M = 1;   // 当前公共模数 lcm
    bool ok = true; // 是否仍有解

    EXCRT() {}

    // 功能: 扩展欧几里得求 a*x + b*y = gcd(a, b)
    static ll exgcd(ll a, ll b, ll& x, ll& y) {
        if (!b) {
            x = 1; y = 0;
            return a;
        }
        ll g = exgcd(b, a % b, y, x);
        y -= a / b * x;
        return g;
    }

    // 功能: 动态并入一个同余方程 x = r (mod m)
    // 传参: r 余数, m 模数
    // 返回: 并入后方程组是否有解
    bool add(ll r, ll m) {
        if (!ok) return false;
        r = (r % m + m) % m;

        ll t, y;
        ll g = exgcd(M, m, t, y);
        ll diff = r - ans;

        if (diff % g != 0) {
            ok = false;
            return false;
        }

        ll mod_step = m / g;
        t = (i128)t * (diff / g) % mod_step;
        if (t < 0) t += mod_step;

        ans += t * M;
        M *= mod_step;
        ans = (ans % M + M) % M;
        return true;
    }

    // --- 一行小封装: 一次性求解方程组 x = r[i] (mod m[i]) ---
    // 返回: {最小非负解 x, lcm 模数 M} (无解返回 {-1, -1})
    static pair<ll, ll> solve(const vector<ll>& r, const vector<ll>& m) {
        EXCRT s;
        for (int i = 0; i < (int)r.size(); ++i) {
            if (!s.add(r[i], m[i])) return {-1, -1};
        }
        return {s.ans, s.M};
    }
};

/*
使用示范:
    // 1. 一次性求解方程组:
    // x = 2 (mod 3), x = 3 (mod 5), x = 2 (mod 7)
    vector<ll> r = {2, 3, 2}, m = {3, 5, 7};
    auto [ans, M] = EXCRT::solve(r, m); // ans = 23, M = 105

    // 2. 动态逐个加入方程:
    EXCRT excrt;
    excrt.add(2, 4); // x = 2 (mod 4)
    excrt.add(4, 6); // x = 4 (mod 6) => ans = 10, M = 12
    if (!excrt.ok) cout << "No Solution\n";
*/
