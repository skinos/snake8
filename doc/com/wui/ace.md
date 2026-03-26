## Ace Admin and this WUI

**Ace Admin** is an open-source **Bootstrap**-based **admin/back-office UI** template: sidebar and top bar, tables, forms, buttons, panels, and other dashboard-style patterns. Reference implementation: [bopoda/ace](https://github.com/bopoda/ace).

**Our device management web UI (WUI)** is built **on top of that approach**—same kind of layout and components (navigation shell, content regions, shared styling). We do **not** ship the full upstream repo as-is; we keep a **product-specific** tree that integrates with the gateway (HE calls, login/session, i18n, `page` helpers, etc.).

**What we use it for, in short:** the **admin shell** around all configuration pages—menu, frame, and reusable UI—and the **feature pages** (network, wireless, LTE, system, …) that read and write settings through scripts such as **`he.js`**.

**Where the Ace-based UI code lives:** all **web pages and assets that follow this Ace Admin–style framework** are under **`project/wui/ace/`**—for example **`content/`** (HTML fragments/pages), **`js/`** (page logic), **`api/`** (e.g. **`he.js`**), **`lang/`** (strings), and **`assets/`** (CSS, images, and related static files used by those pages).

#### **Clone upstream (reference only)**

```
git clone https://github.com/bopoda/ace.git
```

### **Note**

This file is a **framework / layout** guide for HTML, JS, and assets under `project/wui/ace/`. It is **not** a single HE configuration object (`project@component`). For the administration HTTP server object and its **`setup[]` / `shut[]`** lifecycle, see [`admin.md`](admin.md) (**`wui@admin`**).
