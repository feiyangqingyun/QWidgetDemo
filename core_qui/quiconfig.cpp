#include "quiconfig.h"

int QUIConfig::IconMain = 0xf072;
int QUIConfig::IconMenu = 0xf0d7;
int QUIConfig::IconMin = 0xf068;
int QUIConfig::IconMax = 0xf2d2;
int QUIConfig::IconNormal = 0xf2d0;
int QUIConfig::IconClose = 0xf00d;

#ifdef __arm__
QString QUIConfig::FontName = "WenQuanYi Micro Hei";
int QUIConfig::FontSize = 18;
#else
QString QUIConfig::FontName = "Microsoft Yahei";
int QUIConfig::FontSize = 12;
#endif

QString QUIConfig::TextColor = "#000000";
QString QUIConfig::PanelColor = "#F0F0F0";
QString QUIConfig::BorderColor = "#000000";
QString QUIConfig::NormalColorStart = "#F0F0F0";
QString QUIConfig::NormalColorEnd = "#F0F0F0";
QString QUIConfig::DarkColorStart = "#F0F0F0";
QString QUIConfig::DarkColorEnd = "#F0F0F0";
QString QUIConfig::HighColor = "#00BB9E";
