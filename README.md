# 📚 学习养成计划软件（StudyHelper）

一个基于 Qt + SQLite 的桌面应用程序，帮助用户规划、管理和分析学习任务，养成良好的学习习惯。

---

## ✨ 软件功能简介

本软件旨在提供一套系统化的学习任务管理方案，支持任务添加、提醒、打卡、统计分析与图表可视化功能，助力高效学习与时间管理。

### ✅ 主要功能：

- 🗂 **学习任务管理**
  - 支持自定义任务：主题、截止时间、优先级、是否复习
  - 可批量标记完成，生成打卡记录
  - 系统智能推荐学习任务

- ⏰ **提醒功能**
  - 到期提醒：根据截止时间与优先级提醒
  - 复习提醒：基于艾宾浩斯遗忘曲线

- 📈 **数据统计与分析**
  - 饼图展示任务完成率
  - 柱状图展示最近7天每日完成趋势
  - 每周趋势、效率分析报告自动生成

- 🎨 **个性化设置**
  - 主题切换（浅色 / 深色）
  - 提醒频率与复习需求配置

- 💾 **本地数据持久化**
  - 所有任务和打卡记录保存在 SQLite 本地数据库 `studyhelper.db`

---

## 🖼 软件界面预览

| 任务管理界面 | 学习趋势图表 | 设置面板 |
|--------------|---------------|-----------|
| ✅ 支持添加/完成任务 | ✅ 饼图 + 柱状图展示 | ✅ 主题切换、提醒频率 |

（可根据需要上传软件运行截图）

---

## 📦 安装与运行方式

### 编译环境

- Qt 5 或 Qt 6（推荐 Qt 5.15 以上）
- C++17 支持
- 开发工具：Qt Creator / Visual Studio / CLion 等

### 编译步骤

```bash
git clone https://github.com/tsingsuan3009/StudyHelper.git
cd StudyHelper
open StudyHelper.pro  # 或使用 Qt Creator 打开
# 编译并运行即可
