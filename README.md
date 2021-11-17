﻿
#### 一、特别说明
1. 可以选择打开QWidgetDemo.pro一次性编译所有的，也可以到目录下打开pro编译。
2. 如果发现有些子项目没有加载请打开对应目录下的.pro仔细看里面的注释。
3. 编译好的可执行文件在源码同级目录下的bin目录。
4. 亲测Qt4.6到Qt6.2所有版本，亲测win、linux、mac、uos等系统。
5. 有少部分项目不支持部分Qt版本比如Qt6，会自动跳过。
6. **本项目开源的是下面表格中描述的内容，并未开源左侧的树状导航菜单控件，只是方便演示效果图直接用的自定义控件大全的效果图，懒得重新截图。**
7. **建议用git命令行下载，压缩包下载后重新解压文件可能会有问题，不知为何。**

#### 二、目录说明
**高质量项目加粗显示**
| 目录 | 类别 | 名称 | 描述 |
| :------ | :------ | :------ | :------ |
| control | 控件类别 | battery | **电池电量** |
| control | 控件类别 | devicebutton | **设备按钮** |
| control | 控件类别 | devicesizetable | 磁盘容量 |
| control | 控件类别 | imageswitch | 图片开关 |
| control | 控件类别 | ipaddress | IP地址输入框 |
| control | 控件类别 | lightbutton | 高亮按钮 |
| control | 控件类别 | navbutton | **导航按钮** |
| control | 控件类别 | savelog | **日志重定向输出** |
| control | 控件类别 | saveruntime | **运行时间记录** |
| control | 控件类别 | smoothcurve | 平滑曲线 |
| control | 控件类别 | zhtopy | 汉字转拼音 |
| tool | 工具类别 | comtool | **串口调试助手** |
| tool | 工具类别 | nettool | **网络调试助手** |
| tool | 工具类别 | netserver | **网络中转服务器** |
| tool | 工具类别 | base64helper | 图片文字转base |
| tool | 工具类别 | countcode | **代码行数统计工具** |
| tool | 工具类别 | emailtool | 邮件发送工具 |
| tool | 工具类别 | moneytool | 存款利息计算器 |
| tool | 工具类别 | pngtool | 图片警告去除工具 |
| tool | 工具类别 | keytool | **秘钥生成器** |
| tool | 工具类别 | keydemo | 秘钥测试程序 |
| tool | 工具类别 | livetool | 程序启动器 |
| tool | 工具类别 | livedemo | 程序启动示例 |
| video | 视频播放 | videobox | **视频监控布局** |
| video | 视频播放 | videopanel | **视频监控面板** |
| video | 视频播放 | videowidget | 视频监控控件 |
| video | 视频播放 | playffmpeg | 视频播放ffmpeg |
| video | 视频播放 | playvlc | 视频播放vlc |
| video | 视频播放 | plaympv | 视频播放mpv |
| widget | 窗体类别 | colorwidget | 颜色拾取器 |
| widget | 窗体类别 | framelesswidget | **通用无边框窗体** |
| widget | 窗体类别 | gifwidget | GIF录屏 |
| widget | 窗体类别 | lunarcalendarwidget | **农历控件** |
| widget | 窗体类别 | maskwidget | 通用遮罩层 |
| widget | 窗体类别 | movewidget | 通用控件移动 |
| widget | 窗体类别 | screenwidget | 屏幕截图 |
| ui | 界面美化 | flatui | 扁平化风格 |
| ui | 界面美化 | styledemo | **三套风格样式** |
| ui | 界面美化 | uidemo01 | 界面美化基础示例 |
| ui | 界面美化 | uidemo08 | **界面美化入门示例** |
| ui | 界面美化 | uidemo09 | 九宫格主界面 |
| ui | 界面美化 | uidemo10 | **扁平化主界面** |
| other | 其他类别 | bgdemo | 异形窗体 |
| other | 其他类别 | dbpage | **通用数据库翻页** |
| other | 其他类别 | echartgauge | **echart图表js交互** |
| other | 其他类别 | lineeditnext | 文本框回车自动跳转 |
| other | 其他类别 | mouseline | 鼠标十字线 |
| other | 其他类别 | ntpclient | NTP校时 |
| other | 其他类别 | trayicon | 通用托盘效果 |
| third | 第三方类 | designer | QtDesigner设计师(Qt4) |
| third | 第三方类 | hotkey | 全局热键1 |
| third | 第三方类 | shortcut | 全局热键2 |
| third | 第三方类 | miniblink | **浏览器miniblink内核** |
| third | 第三方类 | qwtdemo | 无插件qwt示例 |
| third | 第三方类 | qcustomplotdemo | **精美图表qcustomplot示例** |
| netfriend | 网友提供 | astackwidget | 动态StackWidget |
| netfriend | 网友提供 | imagecropper | **头像设置工具** |
| netfriend | 网友提供 | imageviewwindow | 图片3D效果切换 |
| netfriend | 网友提供 | slidepuzzlewidget | 滑块图片验证码 |

#### 三、学习群
1. **Qt交流大会群 853086607(雨田哥)**
2. **Qt技术交流群 46679801(3000人群)**
3. **Qt进阶之路群 734623697(武威的涛哥)**

#### 四、效果图
##### 4.1、控件类别
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/battery.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/devicebutton.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/devicesizetable.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/imageswitch.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/ipaddress.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/lightbutton.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/navbutton.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/savelog.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/saveruntime.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/smoothcurve.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/control/0snap/zhtopy.jpg)

##### 4.2、工具类别
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/comtool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/nettool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/netserver.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/netserver2.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/base64helper.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/countcode.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/emailtool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/moneytool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/pngtool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/keytool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/keydemo.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/livetool.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/tool/0snap/livedemo.jpg)

##### 4.3、视频播放
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/videobox.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/videopanel.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/videowidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/playffmpeg.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/playvlc.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/video/0snap/plaympv.jpg)

##### 4.4、窗体类别
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/colorwidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/framelesswidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/gifwidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/lunarcalendarwidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/maskwidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/movewidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/widget/0snap/screenwidget.jpg)

##### 4.5、界面美化
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/flatui.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/styledemo1.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/styledemo2.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/styledemo3.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/uidemo01.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/uidemo08.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/uidemo09.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/ui/0snap/uidemo10.jpg)

##### 4.6、其他类别
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/bgdemo.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/dbpage.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/echartgauge.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/lineeditnext.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/mouseline.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/ntpclient.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/other/0snap/trayicon.jpg)

##### 4.7、第三方类
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/designer.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/hotkey.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/shortcut.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/miniblink.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/qwtdemo.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/third/0snap/qcustomplotdemo.jpg)

##### 4.8、网友提供
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/netfriend/0snap/astackwidget.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/netfriend/0snap/imagecropper.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/netfriend/0snap/imageviewwindow.jpg)
![avatar](https://github.com/feiyangqingyun/QWidgetDemo/raw/master/netfriend/0snap/sliderpuzzlewidget.jpg)