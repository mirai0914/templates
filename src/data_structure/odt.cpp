#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 珂朵莉树 (ODT - Old Driver Tree)
 * 结构: set<array<ll, 3>>, 节点为 {l, r, v} (区间值支持 long long)
 * 核心避坑: 双端点切分必须【先 split(r + 1)，再 split(l)】，防止迭代器失效!
 */
struct ODT {
    set<array<ll, 3>> s;

    ODT(int n, ll v = 0) {
        s.insert({1, n, v});
    }

    // 功能: 切分包含位置 x 的区间为 [l, x-1] 和 [x, r]
    // 传参: x 切分点 (将成为右区间左端点；若超出维护右界则返回 s.end())
    auto split(int x) {
        auto it = s.lower_bound({(ll)x, -1LL, LLONG_MIN});
        if (it != s.end() && (*it)[0] == x) return it;
        if (it == s.begin()) return s.end();
        --it;
        if ((*it)[1] < x) return s.end();
        auto [l, r, v] = *it;
        s.erase(it);
        s.insert({l, x - 1, v});
        return s.insert({x, r, v}).first;
    }

    // 功能: 区间推平赋值 [l, r] = v (维持复杂度核心操作)
    void assign(int l, int r, ll v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }

    // 功能: 区间加 [l, r] += v
    void add(int l, int r, ll v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<array<ll, 3>> tmp;
        for (auto it = itl; it != itr; ++it) {
            tmp.push_back({(*it)[0], (*it)[1], (*it)[2] + v});
        }
        s.erase(itl, itr);
        for (const auto& node : tmp) s.insert(node);
    }

    // 功能: 查询区间 [l, r] 第 k 小值 (1-indexed)
    ll kth(int l, int r, int k) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<array<ll, 2>> a;
        for (auto it = itl; it != itr; ++it) {
            a.push_back({(*it)[2], (*it)[1] - (*it)[0] + 1});
        }
        sort(a.begin(), a.end());
        for (const auto& [val, len] : a) {
            k -= len;
            if (k <= 0) return val;
        }
        return -1;
    }

    // 功能: 查询区间 [l, r] 元素 p 次方和 sum_{i=l}^r (a[i]^p) % mod
    ll sum_pow(int l, int r, int p, ll mod) {
        auto itr = split(r + 1);
        auto itl = split(l);
        auto ksm = [&](ll base, int exp) -> ll {
            ll res = 1 % mod;
            base = (base % mod + mod) % mod;
            while (exp > 0) {
                if (exp & 1) res = (__int128_t)res * base % mod;
                base = (__int128_t)base * base % mod;
                exp >>= 1;
            }
            return res;
        };
        ll ans = 0;
        for (auto it = itl; it != itr; ++it) {
            ll len = (*it)[1] - (*it)[0] + 1;
            ans = (ans + len % mod * ksm((*it)[2], p)) % mod;
        }
        return ans;
    }
};

/*
使用示范:
    ODT odt(100000, 0);
    odt.add(1, 10, 5);
    odt.assign(3, 8, 7);
    ll val = odt.kth(1, 10, 4);
    ll sum2 = odt.sum_pow(1, 10, 2, 1000000007LL);
*/
