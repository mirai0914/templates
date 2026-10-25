#pragma once
#include <bits/stdc++.h>
using namespace std;

/**
 * =========================================================================
 * 珂朵莉树 (ODT - Old Driver Tree / Chtholly Tree)
 * =========================================================================
 * 存储结构: set<array<int, 3>>，每个节点为 {l, r, v}。
 * 适用场景: 含随机区间赋值 (Assign/推平) 操作的序列维护问题。
 * 核心避坑准则 (赛场必记):
 *   所有区间操作涉及双端点 split 时，必须【先 split(r + 1)，再 split(l)】！
 *   否则先 split(l) 得到的迭代器可能在 split(r + 1) 执行 erase 时失效，导致 RE！
 */
struct ODT {
    set<array<int, 3>> s;

    ODT() {}
    ODT(int n, int dft_v = 0) {
        build(n, dft_v);
    }

    // 功能: 清空重置，多测安全
    void clear() {
        s.clear();
    }

    // 功能: 初始化区间 [1, n] 初始单值 dft_v
    // 传参: n 区间长度 (1-indexed), dft_v 默认权值
    void build(int n, int dft_v = 0) {
        s.clear();
        s.insert({1, n, dft_v});
    }

    // 功能: 用 vector 构造 ODT (1-indexed, 使用下标 1 ~ a.size()-1)
    // 传参: a 初始权值向量
    void build(const vector<int>& a) {
        s.clear();
        int n = a.size() - 1;
        for (int i = 1; i <= n; ++i) {
            s.insert({i, i, a[i]});
        }
    }

    // 功能: 切分包含位置 x 的区间为 [l, x-1] 和 [x, r]
    // 传参: x 切分点 (将成为右区间的左端点)
    // 返回: 指向 [x, r] 区间的迭代器
    auto split(int x) {
        auto it = prev(s.lower_bound({x + 1, 0, 0}));
        if ((*it)[0] == x) return it;
        auto [l, r, v] = *it;
        s.erase(it);
        s.insert({l, x - 1, v});
        return s.insert({x, r, v}).first;
    }

    // 功能: 区间推平赋值 [l, r] = v (ODT 维持复杂度的核心)
    // 传参: l, r 区间左右界, v 新赋权值
    void assign(int l, int r, int v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }

    // 功能: 区间加 [l, r] += v
    // 传参: l, r 区间左右界, v 增加的权值
    void add(int l, int r, int v) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<array<int, 3>> tmp;
        for (auto it = itl; it != itr; ++it) {
            tmp.push_back({(*it)[0], (*it)[1], (*it)[2] + v});
        }
        s.erase(itl, itr);
        for (auto& node : tmp) {
            s.insert(node);
        }
    }

    // 功能: 查询区间 [l, r] 第 k 小值 (1-indexed)
    // 传参: l, r 区间, k 名次 (1 <= k <= r - l + 1)
    // 返回: 第 k 小的值
    int kth(int l, int r, int k) {
        auto itr = split(r + 1);
        auto itl = split(l);
        vector<pair<int, int>> a;
        for (auto it = itl; it != itr; ++it) {
            a.emplace_back((*it)[2], (*it)[1] - (*it)[0] + 1);
        }
        sort(a.begin(), a.end());
        for (const auto& [val, len] : a) {
            k -= len;
            if (k <= 0) return val;
        }
        return -1;
    }

    // 功能: 查询区间 [l, r] 元素 p 次方和 sum_{i=l}^r (a[i]^p) % mod
    // 传参: l, r 区间, p 幂次, mod 模数
    // 返回: 区间 p 次方和取模结果
    int sum_pow(int l, int r, int p, int mod) {
        auto itr = split(r + 1);
        auto itl = split(l);
        auto ksm = [&](long long base, int exp) -> long long {
            long long res = 1 % mod;
            base %= mod;
            while (exp > 0) {
                if (exp & 1) res = (__int128_t)res * base % mod;
                base = (__int128_t)base * base % mod;
                exp >>= 1;
            }
            return res;
        };
        long long ans = 0;
        for (auto it = itl; it != itr; ++it) {
            long long len = (*it)[1] - (*it)[0] + 1;
            ans = (ans + len * ksm((*it)[2], p)) % mod;
        }
        return (int)ans;
    }
};

/*
使用示范:
    ODT odt;
    // 1. 初始化区间 [1, 100000] 全为 0
    odt.build(100000, 0);

    // 2. 区间加: [1, 10] += 5
    odt.add(1, 10, 5);

    // 3. 区间推平 (核心): [3, 8] = 7
    odt.assign(3, 8, 7);

    // 4. 查询第 k 小: [1, 10] 第 4 小
    int val = odt.kth(1, 10, 4);

    // 5. 查询区间平方和对 1e9+7 取模
    int sum2 = odt.sum_pow(1, 10, 2, 1000000007);
*/
