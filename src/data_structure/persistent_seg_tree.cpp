#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 主席树 (可持久化权值线段树 - Persistent Segment Tree)
 * 复杂度: 空间 O(N log M) 动态开点, 单次插/查 O(log M), 建树 O(N log N)
 * 特性: 支持 int / ll 输入数组 (1-indexed), 内置离散化与多测自闭环清空。
 */
struct PersistentSegTree {
    struct Node {
        int lc = 0, rc = 0, cnt = 0;
    };

    int n = 0, m = 0;
    vector<int> roots;
    vector<Node> tr;
    vector<ll> vals; // 离散化去重原值表

    PersistentSegTree() {}
    template <typename T>
    PersistentSegTree(const vector<T>& a) {
        build(a);
    }

    // 功能: 在历史版本 p 基础上，值域位置 pos 计数 +1 生成新版本
    // 传参: p 旧版本根编号, l, r 当前值域, pos 值域下标 (1 <= pos <= m)
    int insert(int p, int l, int r, int pos) {
        int u = tr.size();
        tr.push_back(tr[p]);
        tr[u].cnt++;
        if (l == r) return u;
        int mid = l + (r - l) / 2;
        if (pos <= mid) tr[u].lc = insert(tr[p].lc, l, mid, pos);
        else tr[u].rc = insert(tr[p].rc, mid + 1, r, pos);
        return u;
    }

    // 功能: 版本差分二分第 k 小值域下标 (u 为右界版本, v 为左界前驱版本)
    int query(int u, int v, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt - tr[tr[v].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) return query(tr[u].lc, tr[v].lc, l, mid, k);
        return query(tr[u].rc, tr[v].rc, mid + 1, r, k - left_cnt);
    }

    // 功能: 一键构建前缀主席树 (a 须为 1-indexed)
    template <typename T>
    void build(const vector<T>& a) {
        if ((int)a.size() <= 1) {
            n = m = 0;
            roots.assign(1, 0);
            tr.assign(1, Node{});
            vals.clear();
            return;
        }
        n = a.size() - 1;
        roots.assign(n + 1, 0);
        tr.clear();
        tr.reserve(n * 20 + 5);
        tr.emplace_back(); // 0 号空节点

        vals.assign(a.begin() + 1, a.end());
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        for (int i = 1; i <= n; ++i) {
            roots[i] = insert(roots[i - 1], get_pos(a[i]));
        }
    }

    // --- 一行小封装入口 ---
    int insert(int p, int pos) { return insert(p, 1, m, pos); }
    int get_pos(ll x) const { return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1; }
    ll query_kth(int l, int r, int k) const { return vals[query(roots[r], roots[l - 1], 1, m, k) - 1]; }

    // --- 可选扩展 1: 区间值域计数 / 二维数点 ---
    int query_count(int u, int v, int l, int r, int ql, int qr) const {
        if (ql > r || qr < l || ql > qr) return 0;
        if (ql <= l && r <= qr) return tr[u].cnt - tr[v].cnt;
        int mid = l + (r - l) / 2;
        return query_count(tr[u].lc, tr[v].lc, l, mid, ql, qr) +
               query_count(tr[u].rc, tr[v].rc, mid + 1, r, ql, qr);
    }
    int count_le(int l, int r, ll x) const {
        int qr = upper_bound(vals.begin(), vals.end(), x) - vals.begin();
        return query_count(roots[r], roots[l - 1], 1, m, 1, qr);
    }
    int count_range(int l, int r, ll x, ll y) const {
        if (x > y) return 0;
        int ql = lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
        int qr = upper_bound(vals.begin(), vals.end(), y) - vals.begin();
        return query_count(roots[r], roots[l - 1], 1, m, ql, qr);
    }

    // --- 可选扩展 2: 树上路径第 k 小 (u + v - lca - fa_lca 差分) ---
    int query_tree(int u, int v, int lca, int fa_lca, int l, int r, int k) const {
        if (l == r) return l;
        int left_cnt = tr[tr[u].lc].cnt + tr[tr[v].lc].cnt - tr[tr[lca].lc].cnt - tr[tr[fa_lca].lc].cnt;
        int mid = l + (r - l) / 2;
        if (k <= left_cnt) return query_tree(tr[u].lc, tr[v].lc, tr[lca].lc, tr[fa_lca].lc, l, mid, k);
        return query_tree(tr[u].rc, tr[v].rc, tr[lca].rc, tr[fa_lca].rc, mid + 1, r, k - left_cnt);
    }
    ll query_path_kth(int u, int v, int lca, int fa_lca, int k) const {
        return vals[query_tree(roots[u], roots[v], roots[lca], roots[fa_lca], 1, m, k) - 1];
    }
};

/*
使用示范:
    vector<int> a = {0, 3, -1, 5, 2, 4, 2, 5, 10, -5}; // 1-indexed
    PersistentSegTree pst(a);
    ll val = pst.query_kth(2, 6, 3);
    int cnt = pst.count_range(2, 6, 2, 4);
*/
