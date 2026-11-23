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
 * PBDS (Policy-Based Data Structures · GCC 扩展数据结构)
 * =========================================================================
 * 适用场景:
 *   1. 平衡树 (tree / ordered_set): O(log N) 动态维护名次树、求第 k 小、求排名。
 *   2. 防卡哈希表 (gp_hash_table): 常数约为 unordered_map 的 1/3~1/5，防 CF 哈希 Hack。
 *   3. 可并配对堆 (priority_queue / heap): O(1) 合并堆 (join)，支持 O(log N) 修改与任意删除。
 * 
 * 核心避坑准则 (赛场必记):
 *   1. 平衡树索引语义:
 *      • find_by_order(k): 0-indexed！k = 0 对应第 1 小元素；越界返回 end()。
 *      • order_of_key(x): 严格小于 x 的元素个数。
 *   2. 平衡树多重集 (Multiset) 致命陷阱:
 *      • 严禁传 less_equal<T>！会导致 lower_bound 语义颠倒为 upper_bound，且 erase(val) 失效。
 *      • 正确做法: 使用模板自带的 OrderedMultiSet 封装，或存储 array<T, 2>{val, ++uid}。
 *   3. 哈希表防 Hack:
 *      • 严禁直接使用默认哈希（会被 107897 等特征值卡至 O(N^2) TLE）。
 *      • 必须搭配 custom_hash (基于 splitmix64 + 运行期时间戳)。
 *   4. 可并堆 join:
 *      • h1.join(h2) 执行后 h2 会被彻底清空，不能再访问旧迭代器。
 */

// =============================================================================
// 模块 1: 平衡树 (Order Statistics Tree - 单值无重)
// =============================================================================
template <typename T>
using ordered_set = tree<
    T,
    null_type,
    less<T>,
    rb_tree_tag,
    tree_order_statistics_node_update
>;

// =============================================================================
// 模块 2: 支持重复元素的高级平衡树封装 (OrderedMultiSet)
// =============================================================================
template <typename T>
struct OrderedMultiSet {
    int uid = 0;
    ordered_set<array<T, 2>> tr;

    OrderedMultiSet() {}

    // 功能: 清空多重平衡树 (多测安全)
    void clear() {
        tr.clear();
        uid = 0;
    }

    // 功能: 插入一个元素 val (允许重复)
    // 传参: val 待插入的值
    void insert(T val) {
        tr.insert({val, ++uid});
    }

    // 功能: 删除一个值为 val 的元素 (若存在多个只删其一)
    // 传参: val 待删除的值
    // 返回: 是否成功删除
    bool erase_one(T val) {
        auto it = tr.lower_bound({val, 0});
        if (it != tr.end() && (*it)[0] == val) {
            tr.erase(it);
            return true;
        }
        return false;
    }

    // 功能: 查询 val 在集合中的排名 (1-indexed: 严格小于 val 的元素个数 + 1)
    // 传参: val 目标值
    // 返回: 1-indexed 排名
    int rank(T val) const {
        return tr.order_of_key({val, 0}) + 1;
    }

    // 功能: 查询集合中第 k 小的元素值 (1-indexed)
    // 传参: k 排名 (1 <= k <= size())
    // 返回: 第 k 小的值
    T kth(int k) const {
        return (*tr.find_by_order(k - 1))[0];
    }

    // 功能: 查询 val 的前驱 (严格小于 val 的最大值)
    // 传参: val 目标值
    // 返回: array<ll, 2>{是否找到 (1/0), 前驱值}
    array<ll, 2> prev_val(T val) const {
        int rk = tr.order_of_key({val, 0});
        if (rk == 0) return {0, 0};
        return {1, (*tr.find_by_order(rk - 1))[0]};
    }

    // 功能: 查询 val 的后继 (严格大于 val 的最小值)
    // 传参: val 目标值
    // 返回: array<ll, 2>{是否找到 (1/0), 后继值}
    array<ll, 2> next_val(T val) const {
        auto it = tr.lower_bound({val + 1, 0});
        if (it == tr.end()) return {0, 0};
        return {1, (*it)[0]};
    }

    // 功能: 集合元素数量
    int size() const { return tr.size(); }
    bool empty() const { return tr.empty(); }
};

// =============================================================================
// 模块 3: 高性能防卡哈希表 (gp_hash_table + custom_hash)
// =============================================================================
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

// 防卡 Hash Map (对应 unordered_map)
template <typename K, typename V>
using safe_map = gp_hash_table<K, V, custom_hash>;

// 防卡 Hash Set (对应 unordered_set)
template <typename K>
using safe_set = gp_hash_table<K, null_type, custom_hash>;

// =============================================================================
// 模块 4: 可并配对堆 (Pairing Heap - 支持 O(1) 合并与 O(log N) 任意修改/删除)
// =============================================================================
template <typename T, typename Compare = less<T>>
using heap = __gnu_pbds::priority_queue<T, Compare, pairing_heap_tag>;

/*
使用示范:
    // 1. 基础平衡树 (无重元素):
    ordered_set<int> os;
    os.insert(10); os.insert(20);
    int rk = os.order_of_key(20);          // rk = 1 (严格小于 20 的元素有 1 个)
    int val = *os.find_by_order(0);        // val = 10 (0-indexed 第 0 小)

    // 2. 多重平衡树 (支持相同数值 & 删除单次出现):
    OrderedMultiSet<ll> ms;
    ms.insert(10); ms.insert(20); ms.insert(20);
    int r = ms.rank(20);                   // 1-indexed 排名: 2
    ll v = ms.kth(3);                      // 第 3 小元素: 20
    auto pv = ms.prev_val(20);             // {1, 10}
    auto nx = ms.next_val(20);             // {0, 0} (无更大值)
    ms.erase_one(20);                      // 仅删除一个 20

    // 3. 高效防卡哈希表:
    safe_map<ll, int> mp;
    mp[123456789LL] = 1;
    safe_set<ll> st;
    st.insert(987654321LL);

    // 4. 可并堆 (Pairing Heap):
    heap<int> h1, h2;
    h1.push(10); h1.push(30);
    h2.push(20); h2.push(40);
    h1.join(h2);                           // O(1) 将 h2 全部并入 h1，h2 自动置空
    int top_val = h1.top();                // 40

    // 可选: 通过迭代器修改/删除堆中任意元素:
    auto it = h1.push(25);
    h1.modify(it, 50);                     // 将 25 修改为 50
    h1.erase(it);                          // 删除该元素
*/
