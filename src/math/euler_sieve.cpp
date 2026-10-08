#pragma once
#include <vector>

/**
 * =========================================================================
 * 模块 1: 基础欧拉筛 (素数线性筛)
 * =========================================================================
 */
struct EulerSieve {
    int n = 0;
    std::vector<int> prm;
    std::vector<bool> isp; // isp[x] == false 表示 x 是质数 (与主人原版逻辑一致)

    EulerSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    void build(int max_n) {
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);

        if (n >= 0) isp[0] = true;
        if (n >= 1) isp[1] = true;

        for (int i = 2; i <= n; i++) {
            if (!isp[i]) {
                prm.push_back(i);
            }
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) break;
            }
        }
    }

    // 判定 x 是否为质数 (O(1))
    bool is_prime(int x) const {
        if (x < 2 || x > n) return false;
        return !isp[x];
    }
};

/**
 * =========================================================================
 * 模块 2: 扩展欧拉筛 (素数 + 欧拉函数 phi + 莫比乌斯函数 mu)
 * =========================================================================
 * 转移推导备忘 (p 为质数):
 * 1. 欧拉函数 phi:
 *    - phi[p] = p - 1
 *    - 若 i % p == 0: phi[i * p] = phi[i] * p
 *    - 若 i % p != 0: phi[i * p] = phi[i] * (p - 1)
 * 
 * 2. 莫比乌斯函数 mu:
 *    - mu[1] = 1, mu[p] = -1
 *    - 若 i % p == 0: mu[i * p] = 0 (含有平方因子 p^2)
 *    - 若 i % p != 0: mu[i * p] = -mu[i]
 */
struct MultiplicativeSieve {
    int n = 0;
    std::vector<int> prm;
    std::vector<bool> isp;
    std::vector<int> phi;
    std::vector<int> mu;

    MultiplicativeSieve(int max_n = 0) {
        if (max_n > 0) build(max_n);
    }

    void build(int max_n) {
        n = max_n;
        prm.clear();
        isp.assign(n + 1, false);
        phi.assign(n + 1, 0);
        mu.assign(n + 1, 0);

        if (n >= 0) isp[0] = true;
        if (n >= 1) {
            isp[1] = true;
            phi[1] = 1;
            mu[1] = 1;
        }

        for (int i = 2; i <= n; i++) {
            if (!isp[i]) {
                prm.push_back(i);
                phi[i] = i - 1;
                mu[i] = -1;
            }
            for (int p : prm) {
                if (1LL * i * p > n) break;
                isp[i * p] = true;
                if (i % p == 0) {
                    // p 是 i 的最小质因数，i 已经包含质因子 p
                    phi[i * p] = phi[i] * p;
                    mu[i * p] = 0;
                    break;
                } else {
                    // p 与 i 互质，由积性函数性质转移
                    phi[i * p] = phi[i] * (p - 1);
                    mu[i * p] = -mu[i];
                }
            }
        }
    }
};
