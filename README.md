
## 0 前言说明
1. **项目作品：[https://qtchina.blog.csdn.net/article/details/97565652](https://qtchina.blog.csdn.net/article/details/97565652)**
2. **视频主页：[https://space.bilibili.com/687803542](https://space.bilibili.com/687803542)**
3. **网店地址：[https://shop244026315.taobao.com](https://shop244026315.taobao.com)**
4. **联系方式：QQ（517216493）微信（feiyangqingyun）推荐加微信。**
5. **公 众 号：Qt教程（民间）  Qt软件（官方）**
6. **版本支持：所有项目已经全部支持Qt4/5/6所有版本以及后续版本。**
7. 监控作品体验：[https://pan.baidu.com/s/1d7TH_GEYl5nOecuNlWJJ7g](https://pan.baidu.com/s/1d7TH_GEYl5nOecuNlWJJ7g) 提取码：01jf
8. 其他作品体验：[https://pan.baidu.com/s/1ZxG-oyUKe286LPMPxOrO2A](https://pan.baidu.com/s/1ZxG-oyUKe286LPMPxOrO2A) 提取码：o05q
9. 监控系统在线文档：[https://feiyangqingyun.gitee.io/QWidgetDemo/video_system/](https://feiyangqingyun.gitee.io/QWidgetDemo/video_system/)
10. 大屏系统在线文档：[https://feiyangqingyun.gitee.io/QWidgetDemo/bigscreen/](https://feiyangqingyun.gitee.io/QWidgetDemo/bigscreen/)
11. 物联网系统在线文档：[https://feiyangqingyun.gitee.io/QWidgetDemo/iotsystem/](https://feiyangqingyun.gitee.io/QWidgetDemo/iotsystem/)

## 1 特别说明
1. 可以选择打开QWidgetDemo.pro一次性编译所有的，也可以到目录下打开pro编译。
2. 如果发现有些子项目没有加载请打开对应目录下的.pro仔细看里面的注释。
3. 编译好的可执行文件在源码同级目录下的bin目录。
4. 亲测Qt4.6到Qt6.6所有版本，亲测win、linux、mac、uos等系统。
5. 有少部分项目不支持部分Qt版本比如Qt6，会自动跳过。
6. **建议用git命令行下载，压缩包下载后重新解压文件可能会有问题，不知为何。**
8. **强烈推荐各位搭配Qt开发经验一起学习 [https://gitee.com/feiyangqingyun/qtkaifajingyan](https://gitee.com/feiyangqingyun/qtkaifajingyan)**
9. 视频演示 [https://www.bilibili.com/video/BV1kg4y1P7VD](https://www.bilibili.com/video/BV1kg4y1P7VD)
10. **各位有需要购买Qt商业版可以联系Qt中国商务人员微信 zzwdkxx ，支持Qt。**

## 2 目录说明
**高质量项目加粗显示**
| 目录      | 相关     | 名称                | 描述                        |
| :-------: | :------: | :-----------------: | :-------------------------- |
| control   | 控件相关 | battery             | **电池电量**                |
| control   | 控件相关 | devicebutton        | **设备按钮**                |
| control   | 控件相关 | devicesizetable     | 磁盘容量                    |
| control   | 控件相关 | imageswitch         | 图片开关                    |
| control   | 控件相关 | ipaddress           | IP地址输入框                |
| control   | 控件相关 | lightbutton         | 高亮按钮                    |
| control   | 控件相关 | navbutton           | **导航按钮**                |
| control   | 控件相关 | savelog             | **日志重定向输出**          |
| control   | 控件相关 | saveruntime         | **运行时间记录**            |
| control   | 控件相关 | smoothcurve         | 平滑曲线                    |
| control   | 控件相关 | zhtopy              | 汉字转拼音                  |
| tool      | 工具相关 | comtool             | **串口调试助手**            |
| tool      | 工具相关 | nettool             | **网络调试助手**            |
| tool      | 工具相关 | netserver           | **网络中转服务器**          |
| tool      | 工具相关 | base64helper        | **图片文字转base**          |
| tool      | 工具相关 | countcode           | **代码行数统计工具**        |
| tool      | 工具相关 | emailtool           | 邮件发送工具                |
| tool      | 工具相关 | moneytool           | 存款利息计算器              |
| tool      | 工具相关 | pngtool             | 图片警告去除工具            |
| tool      | 工具相关 | keytool             | **秘钥生成器**              |
| tool      | 工具相关 | keydemo             | 秘钥测试程序                |
| tool      | 工具相关 | livetool            | 程序启动器                  |
| tool      | 工具相关 | livedemo            | 程序启动示例                |
| video     | 视频播放 | videobox            | **视频监控布局**            |
| video     | 视频播放 | videopanel          | **视频监控面板**            |
| video     | 视频播放 | videowindow         | 视频监控控件                |
| video     | 视频播放 | playffmpeg          | 视频播放ffmpeg              |
| video     | 视频播放 | playvlc             | 视频播放vlc                 |
| video     | 视频播放 | plaympv             | 视频播放mpv                 |
| widget    | 窗体相关 | colorwidget         | 颜色拾取器                  |
| widget    | 窗体相关 | framelesswidget     | **通用无边框窗体**          |
| widget    | 窗体相关 | gifwidget           | GIF录屏                     |
| widget    | 窗体相关 | lunarcalendarwidget | **农历控件**                |
| widget    | 窗体相关 | maskwidget          | 通用遮罩层                  |
| widget    | 窗体相关 | movewidget          | 通用控件移动                |
| widget    | 窗体相关 | screenwidget        | 屏幕截图                    |
| ui        | 界面美化 | flatui              | 扁平化风格                  |
| ui        | 界面美化 | styledemo           | **三套风格样式**            |
| ui        | 界面美化 | uidemo01            | 界面美化基础示例            |
| ui        | 界面美化 | uidemo08            | **界面美化入门示例**        |
| ui        | 界面美化 | uidemo09            | 九宫格主界面                |
| ui        | 界面美化 | uidemo10            | **扁平化主界面**            |
| ui        | 界面美化 | iconhelper          | **超级图形字体**            |
| other     | 其他相关 | bgdemo              | 异形窗体                    |
| other     | 其他相关 | dbpage              | **通用数据库翻页**          |
| other     | 其他相关 | echartgauge         | **echart图表js交互**        |
| other     | 其他相关 | lineeditnext        | 文本框回车自动跳转          |
| other     | 其他相关 | mouseline           | 鼠标十字线                  |
| other     | 其他相关 | ntpclient           | NTP校时                     |
| other     | 其他相关 | trayicon            | 通用托盘效果                |
| other     | 其他相关 | multobj2slot        | 多对象共用槽                |
| other     | 其他相关 | drawrect            | 随机大量矩形                |
| third     | 第三方类 | designer            | QtDesigner设计师(Qt4)       |
| third     | 第三方类 | hotkey              | 全局热键1                   |
| third     | 第三方类 | shortcut            | 全局热键2                   |
| third     | 第三方类 | miniblink           | **浏览器miniblink内核**     |
| third     | 第三方类 | qwtdemo             | 无插件qwt示例               |
| third     | 第三方类 | qcustomplotdemo     | **精美图表qcustomplot示例** |
| netfriend | 网友提供 | astackwidget        | 动态StackWidget             |
| netfriend | 网友提供 | imagecropper        | **头像设置工具**            |
| netfriend | 网友提供 | imageviewwindow     | 图片3D效果切换              |
| netfriend | 网友提供 | slidepuzzlewidget   | 滑块图片验证码              |

## 3 效果图
### 3.1 控件相关-control
- 电池电量-battery
![](control/0snap/battery.jpg)
- 设备按钮-devicebutton
![](control/0snap/devicebutton.jpg)
- 磁盘容量-devicesizetable
![](control/0snap/devicesizetable.jpg)
- 图片开关-imageswitch
![](control/0snap/imageswitch.jpg)
- IP地址输入框-ipaddress
![](control/0snap/ipaddress.jpg)
- 高亮按钮-lightbutton
![](control/0snap/lightbutton.jpg)
- 导航按钮-navbutton
![](control/0snap/navbutton.jpg)
- 日志重定向输出-savelog
![](control/0snap/savelog.jpg)
- 运行时间记录-saveruntime
![](control/0snap/saveruntime.jpg)
- 平滑曲线-smoothcurve
![](control/0snap/smoothcurve.jpg)
- 汉字转拼音-zhtopy
![](control/0snap/zhtopy.jpg)

### 3.2 工具相关-tool
- 串口调试助手-comtool
![](tool/0snap/comtool.jpg)
- 网络调试助手-nettool
![](tool/0snap/nettool.jpg)
- 网络中转服务器-netserver
![](tool/0snap/netserver.jpg)
![](tool/0snap/netserver2.jpg)
- 图片文字转base-base64helper
![](tool/0snap/base64helper.jpg)
- 代码行数统计-countcode
![](tool/0snap/countcode.jpg)
- 邮件发送工具-emailtool
![](tool/0snap/emailtool.jpg)
- 存款利息计算器-moneytool
![](tool/0snap/moneytool.jpg)
- 图片警告去除工具-pngtool
![](tool/0snap/pngtool.jpg)
- 秘钥生成器-keytool
![](tool/0snap/keytool.jpg)
- 秘钥测试程序-keydemo
![](tool/0snap/keydemo.jpg)
- 程序启动器-livetool
![](tool/0snap/livetool.jpg)
- 程序启动示例-livedemo
![](tool/0snap/livedemo.jpg)

### 3.3 视频播放-video
- 视频监控布局-videobox
![](video/0snap/videobox.jpg)
- 视频监控面板-videopanel
![](video/0snap/videopanel.jpg)
- 视频监控控件-videowindow
![](video/0snap/videowindow.jpg)
- 视频播放ffmpeg-playffmpeg
![](video/0snap/playffmpeg.jpg)
- 视频播放vlc-playvlc
![](video/0snap/playvlc.jpg)
- 视频播放mpv-plaympv
![](video/0snap/plaympv.jpg)

### 3.4 窗体相关-widget
- 颜色拾取器-colorwidget
![](widget/0snap/colorwidget.jpg)
- 通用无边框窗体-framelesswidget
![](widget/0snap/framelesswidget.jpg)
- GIF录屏-gifwidget
![](widget/0snap/gifwidget.jpg)
- 农历控件-lunarcalendarwidget
![](widget/0snap/lunarcalendarwidget.jpg)
- 通用遮罩层-maskwidget
![](widget/0snap/maskwidget.jpg)
- 通用控件移动-movewidget
![](widget/0snap/movewidget.jpg)
- 屏幕截图-screenwidget
![](widget/0snap/screenwidget.jpg)

### 3.5 界面美化-ui
- 扁平化风格-flatui
![](ui/0snap/flatui.jpg)
- 三套风格样式-styledemo
![](ui/0snap/styledemo.jpg)
- 界面美化基础示例-uidemo01
![](ui/0snap/uidemo01.jpg)
- 界面美化入门示例-uidemo08
![](ui/0snap/uidemo08.jpg)
- 九宫格主界面-uidemo09
![](ui/0snap/uidemo09.jpg)
- 扁平化主界面-uidemo10
![](ui/0snap/uidemo10.jpg)
- 超级图形字体-iconhelper
![](ui/0snap/iconhelper1.jpg)
![](ui/0snap/iconhelper2.jpg)

### 3.6 其他相关-other
- 异形窗体-bgdemo
![](other/0snap/bgdemo.jpg)
- 通用数据库翻页-dbpage
![](other/0snap/dbpage.jpg)
- echart图表js交互-echartgauge
![](other/0snap/echartgauge.jpg)
- 文本框回车自动跳转-lineeditnext
![](other/0snap/lineeditnext.jpg)
- 鼠标十字线-mouseline
![](other/0snap/mouseline.jpg)
- NTP校时-ntpclient
![](other/0snap/ntpclient.jpg)
- 通用托盘效果-trayicon
![](other/0snap/trayicon.jpg)
- 多对象共用槽-multobj2slot
![](other/0snap/multobj2slot.jpg)
- 随机大量矩形-drawrect
![](other/0snap/drawrect.jpg)

### 3.7 第三方类-third
- QtDesigner设计师(Qt4)-designer
![](third/0snap/designer.jpg)
- 全局热键1-hotkey
![](third/0snap/hotkey.jpg)
- 全局热键2-shortcut
![](third/0snap/shortcut.jpg)
- 浏览器miniblink内核-miniblink
![](third/0snap/miniblink.jpg)
- 无插件qwt示例-qwtdemo
![](third/0snap/qwtdemo.jpg)
- 精美图表qcustomplot示例-qcustomplotdemo
![](third/0snap/qcustomplotdemo1.jpg)
![](third/0snap/qcustomplotdemo2.jpg)
![](third/0snap/qcustomplotdemo3.jpg)
![](third/0snap/qcustomplotdemo4.jpg)
![](third/0snap/qcustomplotdemo5.jpg)
![](third/0snap/qcustomplotdemo6.jpg)
![](third/0snap/qcustomplotdemo7.jpg)

### 3.8 网友提供-netfriend
- 动态StackWidget-astackwidget
![](netfriend/0snap/astackwidget.jpg)
- 头像设置工具-imagecropper
![](netfriend/0snap/imagecropper.jpg)
- 图片3D效果切换-imageviewwindow
![](netfriend/0snap/imageviewwindow.jpg)
- 滑块图片验证码-slidepuzzlewidget
![](netfriend/0snap/sliderpuzzlewidget.jpg)

## 4 学习群
1. **Qt技术交流群1 573199610(已被封)**
2. **Qt技术交流群2 751439350(新建群)**
