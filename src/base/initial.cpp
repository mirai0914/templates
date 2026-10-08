/**
 * @file initial.cpp
 * @brief XCPC 基础竞赛模版
 * @note 本地编译深搜或深树时建议添加扩大栈空间参数:
 *       g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe
 */

#include <bits/stdc++.h>

#define all(n) (n).begin(), (n).end()
#define rall(n) (n).rbegin(), (n).rend()

using namespace std;
using ldb = long double;
using i128 = __int128_t;

const ldb PI = acos(-1.0L);
const int INF = 2e9 + 10;
const long long LINF = 4e18 + 10;
const long long MOD = 998244353;
const double EPS = 1e-9;

#define int long long

void solve()
{
    int n;
    if (!(cin >> n)) return;
    
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T = 1;
    if (cin >> T) {
        while (T--) solve();
    }
    return 0;
}
