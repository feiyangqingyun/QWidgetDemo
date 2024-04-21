#定义复制文件到目录的函数
#为什么上面不加win{}这种
#因为还有在win/linux上的安卓套件/他并不是win/linux套件
#所以两种命令都执行保证任意系统可用
defineTest(copyToDestDir) {
#取出对应的参数变量
srcFile = $$1
dstPath = $$2

#linux和mac系统拷贝
system($$QMAKE_COPY $$srcFile $$dstPath)

#win上需要转换路径
srcFile2 = $$srcFile
dstPath2 = $$dstPath
srcFile2 ~= s,/,\\,g
dstPath2 ~= s,/,\\,g
system($$QMAKE_COPY $$srcFile2 $$dstPath2)
}

#新建目录/在win上目录不存在的话需要主动新建/linux会自动
defineTest(newPath) {
path = $$1
win32 {
path ~= s,/,\\,g
}
system(mkdir $$path)
}

#引入全志H3芯片依赖(不需要的用户可以删除)
unix:!macx {
contains(QT_ARCH, arm) {
contains(DEFINES, arma7) {
INCLUDEPATH += /usr/local/openssl-1.0.2m-h3-gcc-4.9.2/include
LIBS += -L/usr/local/openssl-1.0.2m-h3-gcc-4.9.2/lib -lssl -lcrypto
LIBS += -L/usr/local/h3_rootfsv -lXdmcp
}}}
