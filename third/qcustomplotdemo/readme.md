按照国际惯例，先吹吹牛，QCustomPlot这个开源图表组件，作者绝对是全宇宙Qt领域的天花板，设计的极其巧妙和精美，各种情况都考虑到了，将每个功能细分到不同的类，每个类负责管理自己的绘制和各种属性参数等，虽然代码量不多但是又极具拓展性，当时阅读完代码的时候，我直接跪了8个小时，膝盖现在还是红的。作者的实力至少是八星斗圣级别，只要一个巴掌就能把斗宗干趴下。

**网页版本：[https://feiyangqingyun.gitee.io/qwidgetdemo/qcustomplotdemo/](https://feiyangqingyun.gitee.io/qwidgetdemo/qcustomplotdemo/)**

## 一、功能特点
1. 同时支持多个版本，比如1.3、2.0、2.1。
2. 官方提供的最新的2.1不支持Qt4，也不支持Qt6.2，所以这里通过define判断Qt套件的版本来加载不同的版本。
3. 提供修复不支持Qt6.2的BUG的头文件，自动选择识别。
4. 官方的示例需要通过修改代码运行对应的demo，有时候想集中测试下效果。
5. 同时集成了所有自带示例，通过精美导航栏切换。
6. 集成了自己学习该模块中的经典示例，比如多轴示例。
7. 后期不断持续更新完善示例。
8. 可以直接编译成安卓和web版本。

## 二、功能模块
### 2.1 平方二次元
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/1.jpg)

### 2.2 简单曲线图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/2.jpg)

### 2.3 正弦散点图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/3.jpg)

### 2.4 数据点样式
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/4.jpg)

### 2.5 数据点图片
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/5.jpg)

### 2.6 曲线条样式
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/6.jpg)

### 2.7 日期数据图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/7.jpg)

### 2.8 纹理画刷图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/8.jpg)

### 2.9 双坐标曲线
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/9.jpg)

### 2.10 对数曲线图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/10.jpg)

### 2.11 动态正弦图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/11.jpg)

### 2.12 环形曲线图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/12.jpg)

### 2.13 垂直柱状图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/13.jpg)

### 2.14 箱形盒须图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/14.jpg)

### 2.15 静态指示线
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/15.jpg)

### 2.16 动态指示线
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/16.jpg)

### 2.17 曲线样式图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/17.jpg)

### 2.18 高级坐标轴
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/18.jpg)

### 2.19 颜色热力图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/19.jpg)

### 2.20 金融曲线图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/20.jpg)

### 2.21 南丁格尔图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/21.jpg)

### 2.22 坐标轴指示
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/22.jpg)

### 2.23 相互作用图
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/23.jpg)

### 2.24 滚动条曲线
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/third/qcustomplotdemo/snap/24.jpg)