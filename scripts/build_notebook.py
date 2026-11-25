import os
import re
import html
import subprocess
import docx
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

REPO_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

BOARDS = [
    {
        "id": "1",
        "cat": "数论",
        "title": "欧拉线性筛 (素数筛 + phi & mu)",
        "complexity": "O(N) 线性",
        "formula": "• φ(p) = p-1; i%p==0 ⇒ φ(ip) = φ(i)*p; 否则 φ(i)*(p-1)\n• μ(1)=1, μ(p)=-1; i%p==0 ⇒ μ(ip) = 0; 否则 -μ(i)",
        "md_formula": "- $\\phi(p) = p - 1$；若 $i \\bmod p = 0$ 则 $\\phi(ip) = \\phi(i) \\cdot p$；否则 $\\phi(ip) = \\phi(i) \\cdot (p - 1)$\n- $\\mu(1) = 1, \\mu(p) = -1$；若 $i \\bmod p = 0$ 则 $\\mu(ip) = 0$；否则 $\\mu(ip) = -\\mu(i)$",
        "file": "src/math/euler_sieve.cpp",
        "keep_includes": False,
    },
    {
        "id": "2",
        "cat": "多项式",
        "title": "快速数论变换 (NTT 卷积 / 快速幂 / 分治连乘)",
        "complexity": "O(N log N) / O(N log² N)",
        "formula": "• MOD = 998244353, 原根 G = 3 (若 1004535809 亦可用 G = 3)\n• mult: 多项式卷积 | poly_pow(a, k, lim): 快速幂截断 | poly_prod: 分治连乘",
        "md_formula": "- 模数固定 $998244353$，原根 $G = 3$\n- `mult`: $\\mathcal{O}(N \\log N)$；`poly_pow`: $\\mathcal{O}(L \\log L \\log k)$；`poly_prod`: $\\mathcal{O}(N \\log^2 N)$",
        "file": "src/math/ntt.cpp",
        "keep_includes": False,
    },
    {
        "id": "3",
        "cat": "组合数学",
        "title": "分拆数 (五边形数定理 + 根号分治 DP)",
        "complexity": "O(N sqrt(N))",
        "formula": "• 五边形数递推: p(n) = ∑ (-1)^(k-1) [p(n - k(3k-1)/2) + p(n - k(3k+1)/2)], 符号 + + - -\n• 根号分治 DP: ≤ B 完全背包; > B 记 g[c][j] 为恰有 c 个 > B 的数, 整体+1/添新数 B+1",
        "md_formula": "- **广义五边形数**: $g_k = \\frac{k(3k \\pm 1)}{2}$，符号规律 $+ + - - + + - -$\n- **根号分治 DP**: $\\le B$ 完全背包；$> B$ 维护恰有 $c$ 个数之和为 $j$（整体 $+1$ 或新增 $B+1$）",
        "file": "src/math/partition.cpp",
        "keep_includes": False,
    },
    {
        "id": "4",
        "cat": "组合数学",
        "title": "斯特林数 (O(N^2) 递推打表 + NTT 单行求解)",
        "complexity": "O(N²) / O(N log N)",
        "formula": "• 普通幂转下降幂: x^n = ∑ {n, k} x^{underline k} | 上升幂: x^{overline n} = ∑ [n, k] x^k\n• 递推: s1[i][j] = s1[i-1][j-1] + (i-1)*s1[i-1][j]; s2[i][j] = s2[i-1][j-1] + j*s2[i-1][j]\n• 卷积容斥: {n, k} = ∑_{j=0}^k (-1)^{k-j}/(k-j)! * (j^n / j!)",
        "md_formula": "- 普通幂转下降幂: $x^n = \\sum_{k=0}^n \\{n, k\\} x^{\\underline{k}}$；上升幂: $x^{\\overline{n}} = \\sum_{k=0}^n [n, k] x^k$\n- 第二类卷积通项: $\\{n, k\\} = \\sum_{j=0}^k \\frac{(-1)^{k-j}}{(k-j)!} \\cdot \\frac{j^n}{j!}$",
        "file": "src/math/stirling.cpp",
        "keep_includes": False,
    },
    {
        "id": "5",
        "cat": "数据结构",
        "title": "珂朵莉树 (ODT - Old Driver Tree)",
        "complexity": "随机 assign O(M log log N)",
        "formula": "• 核心避坑: 双端点切分必须【先 split(r + 1)，再 split(l)】！否则左迭代器可能因 erase 失效导致 RE！\n• 结构 set<array<ll, 3>> {l, r, v}, 权值支持 long long 与负数; split(x > n) 返回 s.end()",
        "md_formula": "- **核心避坑**: 双端点操作必须 **先 `split(r + 1)`，再 `split(l)`**！\n- 存储结构 `set<array<ll, 3>>`，随机 `assign` 下块数期望 $\\mathcal{O}(\\log \\log n)$",
        "file": "src/data_structure/odt.cpp",
        "keep_includes": False,
    },
    {
        "id": "6",
        "cat": "数据结构",
        "title": "主席树 (可持久化权值线段树 / 静态 & 树上第 k 小)",
        "complexity": "空间 O(N log M), 查 O(log M)",
        "formula": "• 核心: roots[r] - roots[l-1] 版本差分二分 | 内置离散化, 支持 vector<int> 与 vector<ll>\n• 扩展: 区间 [x, y] 值域计数 count_range (单趟遍历); 树上差分 u + v - lca - fa_lca",
        "md_formula": "- 前缀版本差分 `roots[r] - roots[l - 1]`，动态开点空间 $\\mathcal{O}(N \\log M)$，支持 `int` / `ll`。\n- 扩展：单趟区间值域计数 `count_range`、树上路径第 $k$ 小 `query_path_kth`。",
        "file": "src/data_structure/persistent_seg_tree.cpp",
        "keep_includes": False,
    },
    {
        "id": "7",
        "cat": "数论",
        "title": "扩展中国剩余定理 (EXCRT - 不互质同余方程组)",
        "complexity": "O(N log lcm)",
        "formula": "• 联立: ans + t*M = r (mod m) ⇒ t*M = r - ans (mod m), g = exgcd(M, m, t, y)\n• 无解: (r - ans) % g != 0 | 步长 mod_step = m/g, 全程使用 i128 防中间溢出!",
        "md_formula": "- 联立 $ans + t \\cdot M \\equiv r \\pmod m$，无解条件 $(r - ans) \\bmod g \\neq 0$\n- 中间乘法与取模全程使用 `i128` (`__int128_t`) 杜绝 64 位溢出。",
        "file": "src/math/excrt.cpp",
        "keep_includes": False,
    },
    {
        "id": "8",
        "cat": "数论",
        "title": "欧拉降幂 (扩展欧拉定理) 与单点大数欧拉函数",
        "complexity": "phi O(sqrt(N)), 幂 O(log exp)",
        "formula": "• 单点求 φ(n): O(sqrt(n)) 试除质因数, φ(n) = n * ∏ (p-1)/p\n• 扩展欧拉降幂 (无需 gcd(a, m)=1): b < φ(m) ⇒ a^b % m; b ≥ φ(m) ⇒ a^(b%φ(m) + φ(m)) % m\n• 幂塔扩展: query_tower 递归降幂 (ksm_ge 用 >= m 标记维持指数有效偏移)",
        "md_formula": "- **单点欧拉函数**: $\\mathcal{O}(\\sqrt{n})$ 试除质因数，$\\phi(n) = n \\prod_{p \\mid n} \\frac{p-1}{p}$\n- **扩展欧拉定理**: $b < \\phi(m)$ 直接算；$b \\ge \\phi(m)$ 取 $a^{(b \\bmod \\phi(m)) + \\phi(m)} \\bmod m$",
        "file": "src/math/euler_power.cpp",
        "keep_includes": False,
    },
    {
        "id": "9",
        "cat": "线性代数",
        "title": "线性基 (Linear Basis & 前缀区间线性基)",
        "complexity": "插入/查 O(B), RREF O(B²)",
        "formula": "• 基础: 秩为 r 可张成 2^r 种值; 含 0 (线性相关/插过 0) 时 k=1 为 0, 第 k 小需 --k\n• 前缀线性基: PrefixLinearBasis 贪心保留最大下标 pos[i], pre[r].query_max(l) O(B) 查区间",
        "md_formula": "- 秩为 $r$ 张成 $2^r$ 种值；存在线性相关则 $0$ 占位第 $1$ 小。`rebuild()` 为行最简形（RREF）。\n- **前缀线性基**: 贪心保留更靠右的基下标 `pos[i]`，单次 $\\mathcal{O}(B)$ 在线求区间 $[l, r]$ 最大异或和。",
        "file": "src/math/linear_basis.cpp",
        "keep_includes": False,
    },
    {
        "id": "10",
        "cat": "数据结构",
        "title": "PBDS (平衡树 ordered_set / 防卡哈希 / 可并配对堆)",
        "complexity": "树 O(log N), 堆 join O(1)",
        "formula": "• 平衡树: order_of_key(x) 严格小于 x 的个数; find_by_order(k) 0-indexed 第 k 小迭代器\n• 多重集避坑: 严禁传 less_equal<T>! 维护重复元素建议存 array<T, 2>{val, ++uid}\n• 防卡哈希: safe_map 使用 splitmix64 + 运行期随机盐; 可并堆 h1.join(h2) 后 h2 置空",
        "md_formula": "- `order_of_key(x)`: 严格小于 $x$ 的个数；`find_by_order(k)`: 0-indexed 第 $k$ 小迭代器。\n- **严禁传 `less_equal<T>`**；哈希防 CF Hack 必须配 `custom_hash`；配对堆 `join` 后原堆置空。",
        "file": "src/data_structure/pbds.cpp",
        "keep_includes": True,
    },
]


def extract_print_code(raw_text, keep_includes=False):
    """Extract clean code for PDF/DOCX/HTML print without boilerplate banners."""
    lines = raw_text.splitlines()
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if s in ("#pragma once", "#include <bits/stdc++.h>", "using namespace std;", ""):
            i += 1
            continue
        if not keep_includes and (s.startswith("using ll =") or s.startswith("using i128 =")):
            i += 1
            continue
        break
    rem = "\n".join(lines[i:])
    rem = re.sub(r"^\s*/\*\*.*?\*/\s*", "", rem, flags=re.S)
    # Remove redundant top using namespace/ll right after pbds headers if keep_includes
    if keep_includes:
        out_lines = []
        skipped_first = False
        for ln in rem.splitlines():
            st = ln.strip()
            if not skipped_first and st in ("using namespace std;", "using ll = long long;"):
                continue
            if st.startswith("using namespace __gnu_pbds"):
                skipped_first = True
            out_lines.append(ln)
        rem = "\n".join(out_lines)
        rem = re.sub(r"\n/\*\*.*?\*/\s*\n", "\n\n", rem, count=1, flags=re.S)
    out_lines = rem.splitlines()
    # Collapse multiple blank lines into at most one, and strip blank lines right after `{`
    cleaned = []
    for ln in out_lines:
        st = ln.strip()
        if st == "":
            if not cleaned or cleaned[-1].strip() == "" or cleaned[-1].rstrip().endswith("{"):
                continue
        cleaned.append(ln)
    return "\n".join(cleaned).strip()


# =========================================================================
# C++ Syntax Highlighter (GitHub Light Print-Friendly Palette)
# =========================================================================
CPP_KEYWORDS = {
    "struct", "class", "template", "typename", "using", "namespace", "const",
    "constexpr", "return", "if", "else", "for", "while", "do", "break",
    "continue", "auto", "true", "false", "nullptr", "public", "private",
    "operator", "sizeof", "move", "swap", "sort", "unique", "prev", "next",
    "lower_bound", "upper_bound", "max", "min"
}
CPP_TYPES = {
    "void", "bool", "int", "long", "unsigned", "ll", "i128", "uint64_t",
    "size_t", "char", "string", "vector", "array", "set", "tree", "null_type",
    "less", "rb_tree_tag", "gp_hash_table", "pairing_heap_tag",
    "tree_order_statistics_node_update", "__int128_t"
}

COLOR_COMMENT = RGBColor(106, 115, 125)
COLOR_PREPROC = RGBColor(149, 56, 0)
COLOR_KEYWORD = RGBColor(180, 30, 42)
COLOR_TYPE    = RGBColor(5, 80, 174)
COLOR_STRING  = RGBColor(10, 48, 105)
COLOR_NUMBER  = RGBColor(5, 80, 174)
COLOR_DEFAULT = RGBColor(31, 35, 40)

TOKEN_RE = re.compile(
    r'(?P<STRING>"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\')'
    r'|(?P<IDENT>[A-Za-z_][A-Za-z0-9_]*)'
    r'|(?P<NUMBER>\b(?:0x[0-9A-Fa-f]+|\d+(?:\.\d+)?(?:ULL|LL|UL|L|U)?)\b)'
    r'|(?P<OTHER>\s+|.)'
)


def tokenize_cpp_lines(code):
    """Yield (line_str, tokens_list[(text, rgb, bold)], in_block_comment_before, in_block_comment_after)."""
    lines = code.splitlines()
    in_block = False
    for ln in lines:
        tokens = []
        i = 0
        n = len(ln)
        before_block = in_block
        if not in_block and ln.lstrip().startswith("#"):
            tokens.append((ln, COLOR_PREPROC, True))
            yield ln, tokens, before_block, in_block
            continue

        while i < n:
            if in_block:
                end = ln.find("*/", i)
                if end == -1:
                    tokens.append((ln[i:], COLOR_COMMENT, False))
                    i = n
                else:
                    tokens.append((ln[i:end + 2], COLOR_COMMENT, False))
                    in_block = False
                    i = end + 2
                continue

            # Not in block comment
            c_line = ln.find("//", i)
            c_blk = ln.find("/*", i)
            # Find earliest comment opener outside strings
            # Scan segment before earliest potential comment, respecting strings
            next_stop = n
            if c_line != -1:
                next_stop = min(next_stop, c_line)
            if c_blk != -1:
                next_stop = min(next_stop, c_blk)

            seg = ln[i:] if next_stop == i else ln[i:next_stop]
            if next_stop == i:
                if c_blk == i:
                    in_block = True
                    end = ln.find("*/", i + 2)
                    if end == -1:
                        tokens.append((ln[i:], COLOR_COMMENT, False))
                        i = n
                    else:
                        tokens.append((ln[i:end + 2], COLOR_COMMENT, False))
                        in_block = False
                        i = end + 2
                    continue
                if c_line == i:
                    tokens.append((ln[i:], COLOR_COMMENT, False))
                    i = n
                    continue

            # Tokenize seg, but if a string crosses past next_stop we should handle full line carefully
            # Simpler & exact: run TOKEN_RE on ln[i:] until comment outside string!
            pos = i
            while pos < n:
                if ln.startswith("//", pos):
                    tokens.append((ln[pos:], COLOR_COMMENT, False))
                    pos = n
                    break
                if ln.startswith("/*", pos):
                    in_block = True
                    end = ln.find("*/", pos + 2)
                    if end == -1:
                        tokens.append((ln[pos:], COLOR_COMMENT, False))
                        pos = n
                    else:
                        tokens.append((ln[pos:end + 2], COLOR_COMMENT, False))
                        in_block = False
                        pos = end + 2
                    break
                m = TOKEN_RE.match(ln, pos)
                if not m:
                    tokens.append((ln[pos], COLOR_DEFAULT, False))
                    pos += 1
                    continue
                kind = m.lastgroup
                val = m.group(0)
                if kind == "STRING":
                    tokens.append((val, COLOR_STRING, False))
                elif kind == "IDENT":
                    if val in CPP_KEYWORDS:
                        tokens.append((val, COLOR_KEYWORD, True))
                    elif val in CPP_TYPES:
                        tokens.append((val, COLOR_TYPE, True))
                    else:
                        tokens.append((val, COLOR_DEFAULT, False))
                elif kind == "NUMBER":
                    tokens.append((val, COLOR_NUMBER, False))
                else:
                    tokens.append((val, COLOR_DEFAULT, False))
                pos = m.end()
            i = pos
        yield ln, tokens, before_block, in_block


def compute_keep_with_next(code):
    """Compute per-line keep_with_next flags so functions & comment blocks never split across pages."""
    parsed = list(tokenize_cpp_lines(code))
    n = len(parsed)
    keep = [False] * n
    depths_after = [0] * n
    depths_before = [0] * n

    cur_depth = 0
    seen_method_at_d1 = False
    cur_method_is_ctor = False
    struct_name = ""
    for idx, (ln, tokens, before_blk, after_blk) in enumerate(parsed):
        depths_before[idx] = cur_depth
        for text, col, _ in tokens:
            if col in (COLOR_COMMENT, COLOR_STRING, COLOR_PREPROC):
                continue
            for ch in text:
                if ch == "{":
                    cur_depth += 1
                elif ch == "}":
                    cur_depth = max(0, cur_depth - 1)
        depths_after[idx] = cur_depth

    for idx in range(n - 1):
        ln, _, before_blk, after_blk = parsed[idx]
        st = ln.strip()
        next_st = parsed[idx + 1][0].strip()

        if depths_before[idx] == 0:
            seen_method_at_d1 = False
            cur_method_is_ctor = False
            m_st = re.match(r"struct\s+([A-Za-z_][A-Za-z0-9_]*)", st)
            if m_st:
                struct_name = m_st.group(1)

        if depths_before[idx] == 1 and depths_after[idx] >= 2:
            cur_method_is_ctor = bool(
                st.startswith("struct ") or (struct_name and re.match(rf"^{struct_name}\s*\(", st))
            )

        # Inside a multi-line block comment /* ... */ (even on blank lines!)
        if after_blk:
            keep[idx] = True
            continue

        # Inside any function / method body (depth >= 2 after line): NEVER split inside a function!
        if depths_after[idx] >= 2:
            keep[idx] = True
            continue

        # Blank line at depth 0 or 1 acts as clean break point, UNLESS inside struct header before first non-ctor method or before `};`
        if st == "":
            if depths_before[idx] == 1 and not seen_method_at_d1:
                keep[idx] = True
            elif next_st == "};":
                keep[idx] = True
            else:
                keep[idx] = False
            continue

        # Mark first non-constructor method done; keep constructor closing brace glued to first real method
        if depths_before[idx] >= 2 and depths_after[idx] == 1:
            if cur_method_is_ctor:
                keep[idx] = True
                continue
            seen_method_at_d1 = True
        elif depths_before[idx] == 1 and depths_after[idx] == 1 and "{" in st and "}" in st:
            if struct_name and re.match(rf"^{struct_name}\s*\(", st):
                keep[idx] = True
                continue
            seen_method_at_d1 = True

        # Single-line comment / closing */ attaches to code below
        if st.startswith("//") or st.endswith("*/"):
            if next_st != "":
                keep[idx] = True
                continue

        # Template / using / #include declarations stick to next line
        if st.startswith("template") or st.startswith("using ") or st.startswith("#include"):
            if next_st != "":
                keep[idx] = True
                continue

        # `struct Foo {` (depth 0 -> 1) sticks to its members
        if depths_before[idx] == 0 and depths_after[idx] >= 1:
            keep[idx] = True
            continue

        # Fields / statements inside struct before first method stay attached
        if depths_before[idx] == 1 and not seen_method_at_d1:
            keep[idx] = True
            continue

        # Never orphan `};`
        if next_st == "};":
            keep[idx] = True
            continue

    return parsed, keep


# =========================================================================
# Word XML Helpers
# =========================================================================
def set_cell_background(cell, fill_hex):
    shading_elm = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    cell._tc.get_or_add_tcPr().append(shading_elm)


def set_cell_margins(cell, top=60, bottom=60, left=110, right=110):
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = OxmlElement("w:tcMar")
    for m, val in [("top", top), ("bottom", bottom), ("left", left), ("right", right)]:
        node = OxmlElement(f"w:{m}")
        node.set(qn("w:w"), str(val))
        node.set(qn("w:type"), "dxa")
        tcMar.append(node)
    tcPr.append(tcMar)


def set_table_width(tbl, width_inches):
    dxa = int(width_inches * 1440)
    tblPr = tbl._tbl.tblPr
    tblW = tblPr.find(qn("w:tblW"))
    if tblW is None:
        tblW = OxmlElement("w:tblW")
        tblPr.append(tblW)
    tblW.set(qn("w:w"), str(dxa))
    tblW.set(qn("w:type"), "dxa")
    tblLayout = OxmlElement("w:tblLayout")
    tblLayout.set(qn("w:type"), "fixed")
    tblPr.append(tblLayout)


def set_cell_borders(cell, top=None, bottom=None, left=None, right=None):
    tcPr = cell._tc.get_or_add_tcPr()
    tcBorders = OxmlElement("w:tcBorders")
    for side, spec in [("top", top), ("bottom", bottom), ("left", left), ("right", right)]:
        el = OxmlElement(f"w:{side}")
        if spec is None:
            el.set(qn("w:val"), "none")
        else:
            val, sz, color = spec
            el.set(qn("w:val"), val)
            el.set(qn("w:sz"), str(sz))
            el.set(qn("w:space"), "0")
            el.set(qn("w:color"), color)
        tcBorders.append(el)
    tcPr.append(tcBorders)


def set_table_borders(tbl, top=None, bottom=None, left=None, right=None, insideH=None, insideV=None):
    tblPr = tbl._tbl.tblPr
    borders = OxmlElement("w:tblBorders")
    for side, spec in [
        ("top", top), ("bottom", bottom), ("left", left),
        ("right", right), ("insideH", insideH), ("insideV", insideV)
    ]:
        el = OxmlElement(f"w:{side}")
        if spec is None:
            el.set(qn("w:val"), "none")
        else:
            val, sz, color = spec
            el.set(qn("w:val"), val)
            el.set(qn("w:sz"), str(sz))
            el.set(qn("w:space"), "0")
            el.set(qn("w:color"), color)
        borders.append(el)
    tblPr.append(borders)


def set_row_cant_split(row):
    trPr = row._tr.get_or_add_trPr()
    trPr.append(OxmlElement("w:cantSplit"))


def set_row_header(row):
    trPr = row._tr.get_or_add_trPr()
    trPr.append(OxmlElement("w:tblHeader"))


def set_p_bottom_border(p, sz=8, color="0969DA"):
    pPr = p._p.get_or_add_pPr()
    pBdr = OxmlElement("w:pBdr")
    btm = OxmlElement("w:bottom")
    btm.set(qn("w:val"), "single")
    btm.set(qn("w:sz"), str(sz))
    btm.set(qn("w:space"), "2")
    btm.set(qn("w:color"), color)
    pBdr.append(btm)
    pPr.append(pBdr)


def set_p_top_border(p, sz=4, color="D0D7DE"):
    pPr = p._p.get_or_add_pPr()
    pBdr = OxmlElement("w:pBdr")
    top = OxmlElement("w:top")
    top.set(qn("w:val"), "single")
    top.set(qn("w:sz"), str(sz))
    top.set(qn("w:space"), "2")
    top.set(qn("w:color"), color)
    pBdr.append(top)
    pPr.append(pBdr)


def set_code_para_box(p, is_first=False, is_last=False):
    """Apply seamless shaded code-block borders so adjacent paragraphs merge into one box."""
    pPr = p._p.get_or_add_pPr()
    shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="FAFBFD"/>')
    pPr.append(shd)

    pBdr = OxmlElement("w:pBdr")
    specs = {
        "left": ("single", 12, "0969DA"),
        "right": ("single", 4, "D0D7DE"),
        "top": ("single", 4, "D0D7DE") if is_first else ("none", 0, "auto"),
        "bottom": ("single", 4, "D0D7DE") if is_last else ("none", 0, "auto"),
        "between": ("none", 0, "auto"),
    }
    for side, (val, sz, color) in specs.items():
        el = OxmlElement(f"w:{side}")
        el.set(qn("w:val"), val)
        if val != "none":
            el.set(qn("w:sz"), str(sz))
            el.set(qn("w:space"), "4" if side in ("top", "bottom") else "6")
            el.set(qn("w:color"), color)
        pBdr.append(el)
    pPr.append(pBdr)

    p.paragraph_format.left_indent = Inches(0.06)
    p.paragraph_format.right_indent = Inches(0.06)


def add_field(p, instr, font_name="Segoe UI", font_size=8.5, bold=False, color=RGBColor(36, 41, 47)):
    r_begin = p.add_run()
    fc1 = OxmlElement("w:fldChar")
    fc1.set(qn("w:fldCharType"), "begin")
    r_begin._r.append(fc1)

    r_instr = p.add_run()
    it = OxmlElement("w:instrText")
    it.set(qn("xml:space"), "preserve")
    it.text = f" {instr} "
    r_instr._r.append(it)

    r_sep = p.add_run()
    fc2 = OxmlElement("w:fldChar")
    fc2.set(qn("w:fldCharType"), "separate")
    r_sep._r.append(fc2)

    r_val = p.add_run("?")
    r_val.font.name = font_name
    r_val.font.size = Pt(font_size)
    r_val.font.bold = bold
    r_val.font.color.rgb = color

    r_end = p.add_run()
    fc3 = OxmlElement("w:fldChar")
    fc3.set(qn("w:fldCharType"), "end")
    r_end._r.append(fc3)


def add_bookmark(p, name, bm_id):
    start = OxmlElement("w:bookmarkStart")
    start.set(qn("w:id"), str(bm_id))
    start.set(qn("w:name"), name)
    end = OxmlElement("w:bookmarkEnd")
    end.set(qn("w:id"), str(bm_id))
    p._p.insert(0, start)
    p._p.append(end)


# =========================================================================
# Builders for DOCX + PDF, Markdown, and HTML
# =========================================================================
def build_docx_and_pdf():
    docx_path = os.path.join(REPO_DIR, "notebook.docx")
    pdf_path = os.path.join(REPO_DIR, "notebook.pdf")

    doc = docx.Document()

    # A4 Portrait: 8.27" x 11.69", margins 0.42" L/R, 0.52" Top/Bottom => usable width 7.43"
    USABLE_W = 7.43
    sec = doc.sections[0]
    sec.page_width = Inches(8.27)
    sec.page_height = Inches(11.69)
    sec.left_margin = Inches(0.42)
    sec.right_margin = Inches(0.42)
    sec.top_margin = Inches(0.52)
    sec.bottom_margin = Inches(0.52)
    sec.header_distance = Inches(0.22)
    sec.footer_distance = Inches(0.22)
    sec.different_first_page_header_footer = True

    # Normal style compact defaults
    normal = doc.styles["Normal"]
    normal.font.name = "Segoe UI"
    normal._element.rPr.rFonts.set(qn("w:eastAsia"), "Microsoft YaHei")

    # Running Header (Pages 2+) using full-width 2-col table for guaranteed edge alignment
    hdr = sec.header
    hp0 = hdr.paragraphs[0]
    hp0.paragraph_format.space_before = Pt(0)
    hp0.paragraph_format.space_after = Pt(0)
    htbl = hdr.add_table(rows=1, cols=2, width=Inches(USABLE_W))
    set_table_width(htbl, USABLE_W)
    set_table_borders(htbl, bottom=("single", 4, "D0D7DE"))
    for ci, (align, text) in enumerate([
        (WD_ALIGN_PARAGRAPH.LEFT, "XCPC Team Notebook · mirai_39"),
        (WD_ALIGN_PARAGRAPH.RIGHT, "C++20 · 纯血 vector · 自闭环模板"),
    ]):
        hc = htbl.cell(0, ci)
        hc.width = Inches(USABLE_W / 2)
        set_cell_margins(hc, top=0, bottom=30, left=0, right=0)
        hp = hc.paragraphs[0]
        hp.alignment = align
        hp.paragraph_format.space_before = Pt(0)
        hp.paragraph_format.space_after = Pt(0)
        hr = hp.add_run(text)
        hr.font.name = "Segoe UI"
        hr.font.size = Pt(8)
        hr.font.color.rgb = RGBColor(87, 96, 106)
    # Remove extra empty paragraph above table in header
    hp0._element.getparent().remove(hp0._element)

    # Footer on ALL pages using full-width 2-col table
    for ftr in (sec.first_page_footer, sec.footer):
        fp0 = ftr.paragraphs[0]
        ftbl = ftr.add_table(rows=1, cols=2, width=Inches(USABLE_W))
        set_table_width(ftbl, USABLE_W)
        set_table_borders(ftbl, top=("single", 4, "D0D7DE"))

        fc0 = ftbl.cell(0, 0)
        fc0.width = Inches(USABLE_W * 0.6)
        set_cell_margins(fc0, top=35, bottom=0, left=0, right=0)
        p0 = fc0.paragraphs[0]
        p0.alignment = WD_ALIGN_PARAGRAPH.LEFT
        p0.paragraph_format.space_before = Pt(0)
        p0.paragraph_format.space_after = Pt(0)
        rl = p0.add_run("mirai0914/templates · 打印即用版")
        rl.font.name = "Segoe UI"
        rl.font.size = Pt(8)
        rl.font.color.rgb = RGBColor(100, 110, 120)

        fc1 = ftbl.cell(0, 1)
        fc1.width = Inches(USABLE_W * 0.4)
        set_cell_margins(fc1, top=35, bottom=0, left=0, right=0)
        p1 = fc1.paragraphs[0]
        p1.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        p1.paragraph_format.space_before = Pt(0)
        p1.paragraph_format.space_after = Pt(0)
        rr1 = p1.add_run("第 ")
        rr1.font.name = "Segoe UI"
        rr1.font.size = Pt(8)
        rr1.font.color.rgb = RGBColor(100, 110, 120)
        add_field(p1, "PAGE", font_size=8.5, bold=True, color=RGBColor(9, 105, 218))
        rr2 = p1.add_run(" / ")
        rr2.font.name = "Segoe UI"
        rr2.font.size = Pt(8)
        rr2.font.color.rgb = RGBColor(100, 110, 120)
        add_field(p1, "NUMPAGES", font_size=8.5, bold=True, color=RGBColor(36, 41, 47))
        rr3 = p1.add_run(" 页")
        rr3.font.name = "Segoe UI"
        rr3.font.size = Pt(8)
        rr3.font.color.rgb = RGBColor(100, 110, 120)

        fp0._element.getparent().remove(fp0._element)

    # ---------------------------------------------------------------------
    # PAGE 1: Cover Header + Compile Quick-Ref + Exact Page-Number TOC + Checklist
    # ---------------------------------------------------------------------
    tp = doc.add_paragraph()
    tp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    tp.paragraph_format.space_before = Pt(4)
    tp.paragraph_format.space_after = Pt(2)
    tr = tp.add_run("XCPC Team Notebook · 个人算法板子手册")
    tr.font.name = "Segoe UI"
    tr.font.size = Pt(18)
    tr.font.bold = True
    tr.font.color.rgb = RGBColor(31, 35, 40)

    mp = doc.add_paragraph()
    mp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    mp.paragraph_format.space_after = Pt(8)
    mr = mp.add_run("维护者: mirai_39 (mirai0914)  |  目标: CCPC / ICPC 冲银  |  风格: struct 自闭环 · 纯血 vector · 零 memset")
    mr.font.name = "Segoe UI"
    mr.font.size = Pt(8.5)
    mr.font.color.rgb = RGBColor(87, 96, 106)

    # Contest Compile & Global Header Quick-Ref
    q_tbl = doc.add_table(rows=1, cols=1)
    q_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_width(q_tbl, USABLE_W)
    set_table_borders(
        q_tbl,
        top=("single", 4, "D0D7DE"),
        bottom=("single", 4, "D0D7DE"),
        left=("single", 18, "CF222E"),
        right=("single", 4, "D0D7DE"),
    )
    qc = q_tbl.cell(0, 0)
    set_cell_background(qc, "F8FAFC")
    set_cell_margins(qc, top=75, bottom=75, left=120, right=120)
    qp = qc.paragraphs[0]
    qp.paragraph_format.space_before = Pt(0)
    qp.paragraph_format.space_after = Pt(0)
    qp.paragraph_format.line_spacing = 1.25

    items = [
        ("【Windows 本地深搜防爆栈】 ", True, RGBColor(207, 34, 46)),
        ("g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe\n", False, RGBColor(31, 35, 40)),
        ("【缺省前置头文件 & 宏】     ", True, RGBColor(9, 105, 218)),
        ("#include <bits/stdc++.h>  using namespace std;  using ll = long long;  using i128 = __int128_t;\n", False, RGBColor(31, 35, 40)),
        ("【赛场快读快写模板】         ", True, RGBColor(9, 105, 218)),
        ("cin.tie(nullptr)->sync_with_stdio(false);", False, RGBColor(31, 35, 40)),
    ]
    for idx_i, (txt, bld, col) in enumerate(items):
        r = qp.add_run(txt)
        r.font.name = "Segoe UI" if idx_i % 2 == 0 else "Consolas"
        r.font.size = Pt(8.2)
        r.font.bold = bld
        r.font.color.rgb = col

    # Directory Header
    toc_h = doc.add_paragraph()
    toc_h.paragraph_format.space_before = Pt(12)
    toc_h.paragraph_format.space_after = Pt(4)
    toc_h.paragraph_format.keep_with_next = True
    set_p_bottom_border(toc_h, sz=6, color="24292F")
    thr = toc_h.add_run("目录索引 (Contents)")
    thr.font.name = "Segoe UI"
    thr.font.size = Pt(11.5)
    thr.font.bold = True
    thr.font.color.rgb = RGBColor(31, 35, 40)

    # Directory Table with dynamic PAGEREF page numbers
    toc_tbl = doc.add_table(rows=len(BOARDS) + 1, cols=4)
    toc_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_width(toc_tbl, USABLE_W)
    set_table_borders(
        toc_tbl,
        top=("single", 6, "24292F"),
        bottom=("single", 6, "24292F"),
        insideH=("single", 4, "E5E7EB"),
    )

    col_widths = [0.55, 0.90, 5.18, 0.80]
    headers = ["#", "分类", "模板名称", "页码"]
    hdr_row = toc_tbl.rows[0]
    set_row_header(hdr_row)
    set_row_cant_split(hdr_row)
    for ci, htext in enumerate(headers):
        c = hdr_row.cells[ci]
        c.width = Inches(col_widths[ci])
        set_cell_background(c, "F1F5F9")
        set_cell_margins(c, top=55, bottom=55, left=80, right=80)
        p = c.paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER if ci in (0, 1, 3) else WD_ALIGN_PARAGRAPH.LEFT
        p.paragraph_format.space_before = Pt(0)
        p.paragraph_format.space_after = Pt(0)
        r = p.add_run(htext)
        r.font.name = "Segoe UI"
        r.font.size = Pt(8.5)
        r.font.bold = True
        r.font.color.rgb = RGBColor(31, 35, 40)

    for bi, b in enumerate(BOARDS):
        row = toc_tbl.rows[bi + 1]
        set_row_cant_split(row)
        bg = "FFFFFF" if bi % 2 == 0 else "FAFBFD"
        vals = [b["id"], b["cat"], f"{b['title']}  [{b['complexity']}]", None]
        for ci, val in enumerate(vals):
            c = row.cells[ci]
            c.width = Inches(col_widths[ci])
            c.vertical_alignment = WD_ALIGN_VERTICAL.CENTER
            set_cell_background(c, bg)
            set_cell_margins(c, top=48, bottom=48, left=80, right=80)
            p = c.paragraphs[0]
            p.paragraph_format.space_before = Pt(0)
            p.paragraph_format.space_after = Pt(0)
            if ci in (0, 1):
                p.alignment = WD_ALIGN_PARAGRAPH.CENTER
                r = p.add_run(val)
                r.font.name = "Segoe UI"
                r.font.size = Pt(8.5)
                r.font.bold = (ci == 0)
                r.font.color.rgb = RGBColor(9, 105, 218) if ci == 0 else RGBColor(87, 96, 106)
            elif ci == 2:
                p.alignment = WD_ALIGN_PARAGRAPH.LEFT
                r1 = p.add_run(b["title"] + " ")
                r1.font.name = "Segoe UI"
                r1.font.size = Pt(8.5)
                r1.font.bold = True
                r1.font.color.rgb = RGBColor(31, 35, 40)
                r2 = p.add_run(f"[{b['complexity']}]")
                r2.font.name = "Consolas"
                r2.font.size = Pt(7.8)
                r2.font.color.rgb = RGBColor(106, 115, 125)
            else:
                p.alignment = WD_ALIGN_PARAGRAPH.CENTER
                rp = p.add_run("P.")
                rp.font.name = "Segoe UI"
                rp.font.size = Pt(8.5)
                rp.font.bold = True
                rp.font.color.rgb = RGBColor(9, 105, 218)
                add_field(p, f"PAGEREF board_{b['id']} \\h", font_size=8.5, bold=True, color=RGBColor(9, 105, 218))

    # Contest Checklist Box on Page 1
    chk_h = doc.add_paragraph()
    chk_h.paragraph_format.space_before = Pt(14)
    chk_h.paragraph_format.space_after = Pt(4)
    chk_h.paragraph_format.keep_with_next = True
    set_p_bottom_border(chk_h, sz=6, color="24292F")
    chr_run = chk_h.add_run("赛场常见防 WA / RE / TLE 核对清单 (Checklist)")
    chr_run.font.name = "Segoe UI"
    chr_run.font.size = Pt(11)
    chr_run.font.bold = True
    chr_run.font.color.rgb = RGBColor(31, 35, 40)

    chk_tbl = doc.add_table(rows=1, cols=1)
    chk_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    set_table_width(chk_tbl, USABLE_W)
    set_table_borders(
        chk_tbl,
        top=("single", 4, "D0D7DE"),
        bottom=("single", 4, "D0D7DE"),
        left=("single", 18, "1A7F37"),
        right=("single", 4, "D0D7DE"),
    )
    cc = chk_tbl.cell(0, 0)
    set_cell_background(cc, "FAFBFD")
    set_cell_margins(cc, top=80, bottom=80, left=120, right=120)
    cp = cc.paragraphs[0]
    cp.paragraph_format.space_before = Pt(0)
    cp.paragraph_format.space_after = Pt(0)
    cp.paragraph_format.line_spacing = 1.35
    checklist = [
        "1. 多测清空安全: 严禁使用 memset 或按 MAXV 全量清零！全部按照 O(n) 历史范围定向 .assign() / .clear()。\n",
        "2. 溢出排查: 乘法超过 2e9 强转 1LL 或 (i128)；数论中间取模时刻检查负数 (x % M + M) % M。\n",
        "3. 边界检查: n = 0, n = 1, m = 1, 无解 (-1), 树成一条链 (防爆栈), 空集与 0 在线性基/幂塔中的处理。\n",
        "4. STL / PBDS: 区间删除/ODT 先 split(r + 1) 再 split(l)；PBDS ordered_set 用 0-indexed；哈希必带随机盐。\n",
        "5. 对拍习惯: WA 了先写小范围暴力生成器 + 对拍器，定位根因后再改，禁止赛场瞎猜提交吃罚时。",
    ]
    for line_t in checklist:
        cr = cp.add_run(line_t)
        cr.font.name = "Segoe UI"
        cr.font.size = Pt(8.5)
        cr.font.color.rgb = RGBColor(36, 41, 47)

    # Start Content cleanly on Page 2
    doc.add_page_break()

    # ---------------------------------------------------------------------
    # BOARDS 1 .. 10
    # ---------------------------------------------------------------------
    for bi, b in enumerate(BOARDS):
        # Section Header with Bookmark & keep_with_next
        hp = doc.add_paragraph()
        hp.paragraph_format.space_before = Pt(10 if bi > 0 else 0)
        hp.paragraph_format.space_after = Pt(3)
        hp.paragraph_format.keep_with_next = True
        set_p_bottom_border(hp, sz=8, color="0969DA")
        add_bookmark(hp, f"board_{b['id']}", bi + 1)

        r_num = hp.add_run(f"{b['id']}. ")
        r_num.font.name = "Segoe UI"
        r_num.font.size = Pt(11.5)
        r_num.font.bold = True
        r_num.font.color.rgb = RGBColor(9, 105, 218)

        r_title = hp.add_run(b["title"])
        r_title.font.name = "Segoe UI"
        r_title.font.size = Pt(11.5)
        r_title.font.bold = True
        r_title.font.color.rgb = RGBColor(31, 35, 40)

        r_tag = hp.add_run(f"   [{b['cat']} · {b['complexity']}]")
        r_tag.font.name = "Segoe UI"
        r_tag.font.size = Pt(8.5)
        r_tag.font.color.rgb = RGBColor(106, 115, 125)

        # Compact Formula / Pitfall Box (full width, crisp left accent bar, clean on B&W printers)
        if b.get("formula"):
            ftbl = doc.add_table(rows=1, cols=1)
            ftbl.alignment = WD_TABLE_ALIGNMENT.CENTER
            set_table_width(ftbl, USABLE_W)
            set_table_borders(
                ftbl,
                top=("single", 4, "E5E7EB"),
                bottom=("single", 4, "E5E7EB"),
                left=("single", 18, "D4A72C"),
                right=("single", 4, "E5E7EB"),
            )
            set_row_cant_split(ftbl.rows[0])
            fc = ftbl.cell(0, 0)
            set_cell_background(fc, "FFFDF5")
            set_cell_margins(fc, top=48, bottom=48, left=100, right=100)
            fp = fc.paragraphs[0]
            fp.paragraph_format.space_before = Pt(0)
            fp.paragraph_format.space_after = Pt(0)
            fp.paragraph_format.line_spacing = 1.15
            fp.paragraph_format.keep_with_next = True
            fr = fp.add_run(b["formula"])
            fr.font.name = "Consolas"
            fr.font.size = Pt(7.8)
            fr.font.color.rgb = RGBColor(87, 68, 10)

        # Zero-height / small spacing before code block while keeping attachment
        sp = doc.add_paragraph()
        sp.paragraph_format.space_before = Pt(0)
        sp.paragraph_format.space_after = Pt(2)
        sp.paragraph_format.line_spacing = Pt(1)
        sp.paragraph_format.keep_with_next = True

        # Extract & highlight code line-by-line as seamlessly-bordered paragraphs
        raw_path = os.path.join(REPO_DIR, b["file"])
        with open(raw_path, "r", encoding="utf-8") as f:
            raw_code = f.read()
        code = extract_print_code(raw_code, keep_includes=b["keep_includes"])
        parsed_lines, keep_flags = compute_keep_with_next(code)
        total_lines = len(parsed_lines)

        for li, (ln, tokens, _, _) in enumerate(parsed_lines):
            cp = doc.add_paragraph()
            cp.paragraph_format.space_before = Pt(0)
            cp.paragraph_format.space_after = Pt(0)
            cp.paragraph_format.line_spacing = 1.04
            cp.paragraph_format.keep_together = True
            if keep_flags[li]:
                cp.paragraph_format.keep_with_next = True
            set_code_para_box(cp, is_first=(li == 0), is_last=(li == total_lines - 1))

            if not tokens:
                r = cp.add_run(" ")
                r.font.name = "Consolas"
                r.font.size = Pt(7.5)
            else:
                for text, col, bld in tokens:
                    r = cp.add_run(text)
                    r.font.name = "Consolas"
                    r.font.size = Pt(7.5)
                    r.font.bold = bld
                    r.font.color.rgb = col

    doc.save(docx_path)
    print(f"[OK] Generated Word document: {docx_path}")

    # Export to PDF via Microsoft Word COM Automation, updating TOC PAGEREF & PAGE/NUMPAGES fields!
    ps_cmd = f"""
    $docxPath = "{docx_path}"
    $pdfPath = "{pdf_path}"
    try {{
        $word = New-Object -ComObject Word.Application
        $word.Visible = $false
        $word.DisplayAlerts = 0
        $doc = $word.Documents.Open($docxPath)
        $doc.Repaginate()
        $doc.Fields.Update() | Out-Null
        foreach ($sec in $doc.Sections) {{
            foreach ($hdr in $sec.Headers) {{ $hdr.Range.Fields.Update() | Out-Null }}
            foreach ($ftr in $sec.Footers) {{ $ftr.Range.Fields.Update() | Out-Null }}
        }}
        $doc.SaveAs([ref]$pdfPath, [ref]17)
        $doc.Close([ref]$false)
        $word.Quit()
        Write-Output "[OK] Generated PDF with updated TOC & page numbers: $pdfPath"
    }} catch {{
        Write-Error $_.Exception.Message
        exit 1
    }}
    """
    res = subprocess.run(["powershell", "-ExecutionPolicy", "Bypass", "-Command", ps_cmd], capture_output=True, text=True)
    if res.stdout:
        print(res.stdout.strip())
    if res.stderr:
        print(res.stderr.strip())
    if res.returncode != 0:
        sys.exit(res.returncode)


def slugify_md(id_str, title):
    full = f"{id_str}. {title}".lower()
    out = []
    for ch in full:
        if ch.isalnum() or ("\u4e00" <= ch <= "\u9fff") or ch == "-":
            out.append(ch)
        elif ch in (" ", "_"):
            out.append("-")
    return "".join(out)


def build_markdown():
    md_path = os.path.join(REPO_DIR, "notebook.md")
    lines = [
        "# XCPC Team Notebook · 个人板子手册",
        "",
        "> **维护者**: `mirai_39` / `mirai0914`  ",
        "> **目标**: CCPC / ICPC 赛站冲银  ",
        "> **原则**: 纯血 `vector` · `struct` 自闭环封装 · 杜绝裸数组与 `memset` · 纯正竞赛风  ",
        "",
        "---",
        "",
        "## 赛场常备编译命令 (Windows 本地深搜防爆栈)",
        "",
        "```bash",
        "g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe",
        "```",
        "",
        "缺省快速 I/O:",
        "```cpp",
        "cin.tie(nullptr)->sync_with_stdio(false);",
        "```",
        "",
        "---",
        "",
        "## 目录索引",
        "",
    ]
    for b in BOARDS:
        anchor = slugify_md(b["id"], b["title"])
        lines.append(f"- [{b['id']}. {b['title']}](#{anchor})")

    for b in BOARDS:
        raw_path = os.path.join(REPO_DIR, b["file"])
        with open(raw_path, "r", encoding="utf-8") as f:
            code = f.read().strip()
        lines.extend([
            "",
            "---",
            "",
            f"## {b['id']}. {b['title']}",
            "",
            "> **核心公式与备忘**:  ",
        ])
        for fl in b["md_formula"].splitlines():
            lines.append(f"> {fl}  ")
        lines.extend([
            "",
            "```cpp",
            code,
            "```",
        ])

    with open(md_path, "w", encoding="utf-8", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")
    print(f"[OK] Generated Markdown: {md_path}")


def build_html():
    html_path = os.path.join(REPO_DIR, "notebook.html")
    cards_html = []
    for b in BOARDS:
        raw_path = os.path.join(REPO_DIR, b["file"])
        with open(raw_path, "r", encoding="utf-8") as f:
            code = extract_print_code(f.read(), keep_includes=b["keep_includes"])
        f_html = "<br>".join(html.escape(line) for line in b["formula"].splitlines())
        c_html = html.escape(code)
        cards_html.append(f"""
        <!-- {b['id']}. {html.escape(b['title'])} -->
        <div class="section-card">
            <div class="section-header">
                <span>{b['id']}. {html.escape(b['title'])}</span>
                <span class="tag">{html.escape(b['cat'])} · {html.escape(b['complexity'])}</span>
            </div>
            <div class="section-body">
                <div class="formula-box">{f_html}</div>
<pre>{c_html}</pre>
            </div>
        </div>""")

    doc_html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>XCPC Team Notebook - mirai_39</title>
    <style>
        @page {{
            size: A4 portrait;
            margin: 12mm 10mm 12mm 10mm;
            @bottom-right {{
                content: "第 " counter(page) " / " counter(pages) " 页";
                font-size: 8pt;
                font-family: sans-serif;
            }}
        }}
        * {{ box-sizing: border-box; }}
        body {{
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
            color: #1f2328;
            margin: 0 auto;
            padding: 16px;
            max-width: 960px;
            font-size: 9pt;
            line-height: 1.4;
        }}
        .header {{
            text-align: center;
            border-bottom: 2px solid #24292f;
            padding-bottom: 6px;
            margin-bottom: 10px;
        }}
        .header h1 {{ margin: 0 0 4px 0; font-size: 16pt; }}
        .header .meta {{ font-size: 8.5pt; color: #57606a; }}
        .notice-box {{
            background-color: #f8fafc;
            border: 1px solid #d0d7de;
            border-left: 4px solid #cf222e;
            border-radius: 4px;
            padding: 6px 10px;
            margin-bottom: 12px;
            font-size: 8.2pt;
        }}
        .notice-box code {{
            background: #eaeef2;
            padding: 1px 4px;
            border-radius: 3px;
            font-family: Consolas, monospace;
            color: #cf222e;
        }}
        .section-card {{
            break-inside: avoid;
            page-break-inside: avoid;
            margin-bottom: 12px;
            border: 1px solid #d0d7de;
            border-radius: 4px;
            background: #fff;
        }}
        .section-header {{
            background: #f1f5f9;
            padding: 5px 10px;
            font-size: 9.8pt;
            font-weight: bold;
            color: #0969da;
            border-bottom: 1px solid #d0d7de;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }}
        .section-header .tag {{ font-size: 8pt; font-weight: normal; color: #57606a; }}
        .section-body {{ padding: 8px 10px; }}
        .formula-box {{
            background: #fffdf5;
            border: 1px solid #e5e7eb;
            border-left: 3px solid #d4a72c;
            padding: 4px 8px;
            margin-bottom: 8px;
            font-family: Consolas, monospace;
            font-size: 8pt;
            color: #57440a;
        }}
        pre {{
            margin: 0;
            background: #fafbfd;
            border: 1px solid #d0d7de;
            border-left: 3px solid #0969da;
            padding: 6px 8px;
            font-family: Consolas, monospace;
            font-size: 8pt;
            line-height: 1.25;
            white-space: pre-wrap;
            word-break: break-all;
        }}
    </style>
</head>
<body>
    <div class="header">
        <h1>XCPC Team Notebook · 个人板子手册</h1>
        <div class="meta">维护者: mirai_39 / mirai0914 | CCPC / ICPC 冲银 | 纯血 vector · struct 自闭环封装 · 竞赛风</div>
    </div>
    <div class="notice-box">
        <b>【本地防爆栈编译】</b> <code>g++ -O2 -std=c++20 solve.cpp -Wl,--stack=268435456 -o solve.exe</code><br>
        <b>【快速 I/O】</b> <code>cin.tie(nullptr)->sync_with_stdio(false);</code>
    </div>
{"".join(cards_html)}
</body>
</html>
"""
    with open(html_path, "w", encoding="utf-8", newline="\r\n") as f:
        f.write(doc_html)
    print(f"[OK] Generated HTML: {html_path}")


if __name__ == "__main__":
    build_docx_and_pdf()
    build_markdown()
    build_html()
