#pragma execution_character_set("utf-8")

#include "frmslidepuzzlewidget.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setFont(QFont("Microsoft Yahei", 10));

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
	QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
	QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#else
	QTextCodec *codec = QTextCodec::codecForName("utf-8");
	QTextCodec::setCodecForLocale(codec);
#endif

	FrmSlidePuzzleWidget w;
	w.setWindowTitle("滑块图片验证码");
	w.show();
	return a.exec();
}
