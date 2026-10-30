# XCPC Algorithm Templates (个人板子库)

> 维护者: mirai_39 / mirai0914  
> 定位: 仅收录高价值、推导繁琐、难默写的算法模板

---

## 赛前现场打印手册 (Team Notebook)

为避免打印店对 HTML 支持不佳导致排版跑偏或无法打印的问题，现已全量提供标准文印格式：

- 🌟 **[notebook.pdf](notebook.pdf)**（**强烈推荐，打印店首选**）
  - 全球文印店通用标准格式，双击直接用任意阅读器打开打印，字号、行距、分页绝对不会变形。
- 📄 **[notebook.docx](notebook.docx)**（Word 文档备用）
  - 若部分店家仅认 Word / WPS，可直接使用此文件打印。
- 🔄 **一键更新脚本**：
  ```bash
  python scripts/build_notebook.py
  ```
  执行后会自动将 `src/` 下所有算法板子打包并重新生成最新的 `.docx` 与 `.pdf`。
