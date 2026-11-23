#pragma once
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include <ext/pb_ds/priority_queue.hpp>

using namespace std;
using namespace __gnu_pbds;

using ll = long long;

/**
 * =========================================================================
 * PBDS (Policy-Based Data Structures · GCC 扩展库常用类型)
 * =========================================================================
 * 1. 平衡树 (tree / ordered_set): O(log N) 动态名次树
 *    • tr.find_by_order(k): 0-indexed 返回第 k 小元素的迭代器 (k=0 为最小，越界返回 end())
 *    • tr.order_of_key(x):  返回严格小于 x 的元素个数 (即 0-indexed 排名，1-indexed 排名需 +1)
 *    • 注意: 维护多重集严禁写 less_equal<T> (会导致 lower_bound 变 upper_bound 且 erase 失效)，
 *           存 array<T, 2>{val, ++uid} 即可。
 * 
 * 2. 高效哈希表 (gp_hash_table): 探测法寻址，常数远小于 unordered_map
 *    • CF 上默认哈希容易被 Hack，建议搭配 custom_hash 防卡。
 * 
 * 3. 可并配对堆 (priority_queue / heap):
 *    • h1.join(h2): O(1) 将 h2 全部并入 h1，h2 会被清空。
 *    • auto it = h.push(x); h.modify(it, new_val); h.erase(it); (O(log N) 修改与删除)
 */

// 1. 平衡树声明
template <typename T>
using ordered_set = tree<
    T,
    null_type,
    less<T>,
    rb_tree_tag,
    tree_order_statistics_node_update
>;

// 2. 防卡哈希声明
struct custom_hash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

template <typename K, typename V>
using safe_map = gp_hash_table<K, V, custom_hash>;

template <typename K>
using safe_set = gp_hash_table<K, null_type, custom_hash>;

// 3. 可并堆声明 (默认大根堆)
template <typename T, typename Compare = less<T>>
using heap = __gnu_pbds::priority_queue<T, Compare, pairing_heap_tag>;

/*
基础用法示范:
    // --- 平衡树 ---
    ordered_set<int> tr;
    tr.insert(10);
    tr.insert(20);
    tr.insert(30);

    int rk = tr.order_of_key(20);           // 1 (严格小于 20 的元素有 1 个)
    int val = *tr.find_by_order(0);         // 10 (0-indexed 第 0 小)
    auto it = tr.lower_bound(20);           // 第一个 >= 20 的迭代器
    tr.erase(it);                           // 删除元素

    // --- 哈希表 ---
    safe_map<ll, int> mp;
    mp[123456789LL] = 1;
    if (mp.find(123456789LL) != mp.end()) {
        int v = mp[123456789LL];
    }

    // --- 可并堆 ---
    heap<int> h1, h2;
    h1.push(10); h1.push(30);
    h2.push(20); h2.push(40);
    h1.join(h2);                            // O(1) 合并，h2 被置空
    int top_val = h1.top();                 // 40
*/
