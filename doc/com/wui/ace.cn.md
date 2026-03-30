## Ace Admin 与此 WUI

**Ace Admin** 是一个基于 **Bootstrap** 的开源**后台管理 UI** 模板：侧边栏和顶栏、表格、表单、按钮、面板以及其他仪表盘风格的组件。参考实现：[bopoda/ace](https://github.com/bopoda/ace)。

**我们的设备管理 Web UI (WUI)** **基于这种方式构建**——使用相同类型的布局和组件（导航外壳、内容区域、共享样式）。我们**不会**原样发布上游完整仓库；我们维护一个**产品专用**的目录树，与网关集成（HE 调用、登录/会话、国际化、`page` 辅助工具等）。

**简而言之，我们使用它的目的是：** 围绕所有配置页面的**管理外壳**——菜单、框架和可复用 UI——以及通过 **`he.js`** 等脚本读写设置的**功能页面**（网络、无线、LTE、系统等）。

**基于 Ace 的 UI 代码存放位置：** 所有遵循此 Ace Admin 风格框架的 **Web 页面和资源** 都位于 **`project/wui/ace/`** 下——例如 **`content/`**（HTML 片段/页面）、**`js/`**（页面逻辑）、**`api/`**（例如 **`he.js`**）、**`lang/`**（字符串）和 **`assets/`**（CSS、图片以及这些页面使用的相关静态文件）。

#### 克隆上游（仅供参考）

```
git clone https://github.com/bopoda/ace.git
```

### 说明

此文件是 `project/wui/ace/` 下 HTML、JS 和资源的**框架/布局**指南。它**不是**一个单独的 HE 配置对象（`project@component`）。有关管理 HTTP 服务器对象及其 **`setup[]` / `shut[]`** 生命周期，请参见 [`admin.cn.md`](admin.cn.md)（**`wui@admin`**）。
