#pragma once
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

/**
 * 欧拉线性筛 (基础素数筛 + 积性函数 phi & mu)
 * 复杂度: 时间 O(N), 空间 O(N)
 */

// 模块 1: 基础欧拉筛 (素数线性筛)
struct EulerSieve {
    int n = 0;
    vector<int> prm;
    vector<bool> isp; // isp[x] == false 表示 x 是质数

    EulerSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    // 功能: 线性筛预处理 1 ~ max_n 内所有素数
    // 传参: max_n 预处理上界
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);

        if (n >= 0) isp[0] = true;
        if (n >= 1) isp[1] = true;

        for (int i = 2; i <= n; ++i) {
            if (!isp[i]) prm.push_back(i);
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) break;
            }
        }
    }

    // 功能: O(1) 判定 x 是否为质数
    bool is_prime(int x) const {
        if (x < 2 || x > n) return false;
        return !isp[x];
    }
};

// 模块 2: 扩展欧拉筛 (素数 + 欧拉函数 phi + 莫比乌斯函数 mu)
struct MultiplicativeSieve {
    int n = 0;
    vector<int> prm;
    vector<bool> isp;
    vector<int> phi, mu;

    MultiplicativeSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    // 功能: 线性筛同时预处理素数表、phi[1..n] 与 mu[1..n]
    // 传参: max_n 预处理上界
    void build(int max_n) {
        if (max_n < 0) return;
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);
        phi.assign(n + 1, 0);
        mu.assign(n + 1, 0);

        if (n >= 0) isp[0] = true;
        if (n >= 1) {
            isp[1] = true;
            phi[1] = mu[1] = 1;
        }

        for (int i = 2; i <= n; ++i) {
            if (!isp[i]) {
                prm.push_back(i);
                phi[i] = i - 1;
                mu[i] = -1;
            }
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) {
                    phi[i * p] = phi[i] * p;
                    mu[i * p] = 0;
                    break;
                } else {
                    phi[i * p] = phi[i] * (p - 1);
                    mu[i * p] = -mu[i];
                }
            }
        }
    }

    // 功能: O(1) 判定 x 是否为质数
    bool is_prime(int x) const {
        if (x < 2 || x > n) return false;
        return !isp[x];
    }
};

/*
使用示范:
    EulerSieve es(1000000);
    bool ok = es.is_prime(998244353);

    MultiplicativeSieve ms(1000000);
    int p10 = ms.phi[10]; // 4
    int m6 = ms.mu[6];    // 1
*/
