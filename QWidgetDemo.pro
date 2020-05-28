TEMPLATE = subdirs
#定义了ordered表示子项目按照添加的顺序来编译
CONFIG  += ordered
SUBDIRS += lightbutton      #高亮按钮控件
SUBDIRS += movewidget       #通用控件移动类
SUBDIRS += flatui           #模仿flatui类
SUBDIRS += countcode        #代码统计组件
SUBDIRS += gifwidget        #屏幕录制控件
SUBDIRS += comtool          #串口调试助手
SUBDIRS += nettool          #网络调试助手
SUBDIRS += devicesizetable  #硬盘容量控件
SUBDIRS += styledemo        #高仿PS黑色+扁平白色+淡蓝色风格主题
SUBDIRS += navbutton        #导航按钮控件
SUBDIRS += videopanel       #视频监控画面分割demo
SUBDIRS += framelesswidget  #通用无边框拖动拉伸类
SUBDIRS += ipaddress        #IP地址输入控件
SUBDIRS += bgdemo           #无边框背景透明窗体
SUBDIRS += dbpage           #通用数据库翻页查询
SUBDIRS += pngtool          #PNG图片警告去除工具
SUBDIRS += savelog          #日志重定向输出类
SUBDIRS += saveruntime      #运行时间记录类
SUBDIRS += colorwidget      #颜色拾取器
SUBDIRS += maskwidget       #遮罩层窗体
SUBDIRS += battery          #电池电量控件
SUBDIRS += lineeditnext     #文本框回车焦点下移
SUBDIRS += zhtopy           #汉字转拼音
SUBDIRS += qwtdemo          #qwt的源码版本，无需插件，直接源码集成到你的项目即可
SUBDIRS += buttondefence    #通用按钮地图效果
SUBDIRS += mouseline        #鼠标定位十字线
SUBDIRS += emailtool        #邮件发送工具
SUBDIRS += ntpclient        #NTP服务器时间同步
SUBDIRS += lunarcalendarwidget#农历控件
SUBDIRS += videowidget      #通用视频控件
SUBDIRS += screenwidget     #屏幕截图控件
SUBDIRS += imageswitch      #图片开关控件

#如果你电脑对应的Qt版本有webkit或者webengine组件可以自行打开
#SUBDIRS += echartgauge      #echart仪表盘含交互支持webkit及webengine
#ffmpegdemo默认提供的win的lib,如果是win可以自行打开
#SUBDIRS += ffmpegdemo       #视频流播放ffmpeg内核
#vlcdemo默认提供的win的lib,如果是win可以自行打开
#SUBDIRS += vlcdemo          #视频流播放vlc内核
#designer项目只支持Qt4,如果是Qt4可以自行打开
#SUBDIRS += designer         #QtDesigner4源码

SUBDIRS += netserver        #网络中转服务器
