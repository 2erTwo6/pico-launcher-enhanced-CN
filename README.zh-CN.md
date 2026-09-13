# Pico Launcher Enhanced 中文版

[![最新版本](https://img.shields.io/github/v/release/rasalopa/pico-launcher-enhanced?display_name=tag&sort=semver&label=release)](../../releases/latest)
[![License](https://img.shields.io/github/license/rasalopa/pico-launcher-enhanced)](LICENSE.txt)

这是 [Pico Launcher](https://github.com/LNH-team/pico-launcher) 的增强分支，加入了收藏、
通关标记、游戏统计、最近游玩等现代主机风格的功能，同时兼容原版 SD 卡目录结构。
本分支在原版基础上完成了简体中文汉化，并使用点阵字体显示中文、英文、数字和符号。

完整的功能说明、安装步骤和构建方式请参阅 [英文 README](README.md) 或 [增强功能文档](docs/Enhanced.md)。

## 汉化与点阵字体

- 启动器界面已汉化为简体中文。
- 所有字符——中文、英文字母、数字、标点——都由同一套
  `arm9/data/BitmapSong-9pt.nft2` 点阵字体绘制。
- 字体来源为文泉驿点阵宋体 9pt（WenQuanYi Bitmap Song），通过
  `tools/make_chinese_bitmap_font.py` 转换为 Nitro Font 2 格式。
- 字体采用 GNU GPL v2 + 字体嵌入例外授权，详见
  [`licenses/wqy-bitmap-song.txt`](licenses/wqy-bitmap-song.txt)。

点阵字体包含常用 ASCII、完整 GB2312 汉字、中文标点和全角字符，因此中文
ROM 文件名、金手指名称和界面文字都可以直接显示。

## 安装

1. 下载或自行构建 `LAUNCHER.nds`。
2. 将其重命名为 `_picoboot.nds`，放到 SD 卡根目录。
3. 确保 `_pico` 目录中包含 `aplist.bin`、`savelist.bin`、`picoLoader7.bin`
   和 `picoLoader9.bin`。

原版主题、封面和 `_pico` 目录内容均可继续使用。

## 构建

推荐使用 BlocksDS 工具链。Windows 可用 WSL 或 MSYS2；也可以直接使用作者
提供的 Docker 镜像：

```sh
docker run --rm -v "$PWD":/work -w /work skylyrac/blocksds:slim-v1.16.0 make
```

生成的 ROM 位于仓库根目录：`LAUNCHER.nds`。

## 主要功能

- 收藏、通关标记、收藏和通关筛选
- 最近游玩列表、游戏统计、累计游玩时间
- 按首字母跳转、随机启动、删除游戏与存档
- 多种浏览布局、封面/横幅/图标模式、亮度调节
- 隐藏空文件夹、文件夹背景音乐、昼夜主题背景
- 金手指列表一键全关、截图到 `/_pico/screenshots`

更多细节见 [增强功能文档](docs/Enhanced.md)、[使用说明](docs/Usage.md) 和
[自定义主题](docs/Themes.md)。
