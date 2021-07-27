### 一 前言
不知道各位程序员有没有遇到过这样一种困惑，好不容易在开源网站找到了类似的想要的项目代码，结果down下来一编译，我勒个去，几百个错误，根本没法用，熟悉的人还好可以直接阅读代码进行修改（有些只要做小改动就能正常编译，有些可能需要大刀阔斧），可是对于绝大部分的初学者来说绝对是噩梦，连把代码看下去的勇气都没有，我们没有任何权利和理由去责怪开原作者，只是期望各位能够在项目开源的同时，将开源项目完善好、测试好，最起码要把项目中依赖的遗漏的文件一起打包好，或者作出必要的说明，比如对应的开发编译版本要求，以来的文件去哪里下载。很多优秀的项目就毁在这个地方，没人完善和维护（可能因为没有耐心，也没有持续的收入来源，所以干的没劲，作者甚至转行送外卖了）。只有解决了这个痛点才能使得对应的开源项目持续发光发热。

本人自从学习Qt开发以来，开源过至少上百个项目（大部分早期开源的目前不在开源主页，有空会全部整理好重新发布），我要是说在国内Qt界开源的项目数量和质量排第十的话，没人敢说排第一、第二、第三...到第九。关于无边框界面方案，网上也有不少的优秀的开源的例子，99%都存在以下几个问题，针对以上问题，解决这些痛点，借助自己刚好有多个操作系统、几十个Qt版本的开发测试环境，特意完善了这个无边框类。

- 无法正常编译，缺少文件，作者真粗心，自己都没测试过。
- 只限定了部分特定的版本才能编译。
- 只解决了单个问题，比如无边框拖动，没有系统特性拉到左侧右侧半屏、顶部最大化。
- 代码赶鸭子上架，复制粘贴的一坨坨，毫无章法。
- 代码就是给作者自己用的，放上去就是给个参考，管他那么多。
- 往左侧拉动抖动的厉害。
- mac系统上不能最小化。
- 不能同时支持win、linux、mac三种主流操作系统。

### 二 功能特点
1. 同时支持Qt4-Qt6，亲测Qt4.7到Qt6.2以及后续版本。
2. 同时支持mingw、msvc、gcc等。
3. 同时支持windows、linux、mac。
4. 同时支持QMainWindow、QWidget、QDialog。
5. 使用方法极其简单，只需要将继承类修改即可。
6. 自动识别双击标题栏响应。
7. 无边框拉伸在windows下不抖动。
8. 在windows下具有移动到边缘半屏、移动到顶部全屏特性。
9. 解决mac系统上无边框最小化最大化失效的BUG。
10. 解决系统休眠后再次启动程序懵逼的BUG。
11. 解决有时候窗体重新显示的时候假死不刷新的BUG。
12. 轻量级，1个代码文件，核心代码行数不到300行。
13. 注释详细，示例完美，非常适合阅读和学习。
14. 开源开箱即用，保证任意Qt版本可正常编译运行，无需任何调整。

### 三 效果图
#### 1 windows
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/frameless/snap/win.gif)

#### 2 ubuntu
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/frameless/snap/ubuntu.gif)

#### 3 uos
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/frameless/snap/uos.gif)

#### 4 kylin
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/frameless/snap/kylin.gif)

#### 5 mac
![avatar](https://gitee.com/feiyangqingyun/QWidgetDemo/raw/master/frameless/snap/mac.gif)

### 四 特别说明
1. 点赞、评论、留言、转发、发代码。
2. 国内站点：[https://gitee.com/feiyangqingyun/QWidgetDemo](https://gitee.com/feiyangqingyun/QWidgetDemo)
3. 国际站点：[https://github.com/feiyangqingyun/QWidgetDemo](https://github.com/feiyangqingyun/QWidgetDemo)