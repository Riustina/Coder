#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QHash>
#include <QChar>
#include <QString>
#include <QHBoxLayout>

namespace Ui {
class Widget;
}

struct EncodingInfo {
    QString utf8Hex;     // UTF-8 编码（十六进制）
    QString utf16Hex;    // UTF-16 编码（十六进制）
    QString unicodeHex;  // Unicode 编码（十六进制）
    QString color;       // 背景颜色（样式表格式）
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onConvertButtonClicked(); // 按钮点击槽函数

private:
    Ui::Widget *ui;
    QHash<QChar, EncodingInfo> encodingMap; // 字符到编码信息的映射
    QHBoxLayout *originalLayout;  // 原文显示的水平布局
    QHBoxLayout *encodingLayout;  // 编码显示的水平布局

    void clearContainerLayout(QHBoxLayout *layout); // 清除布局中的控件
    QString generateRandomColor();  // 生成随机背景颜色
    QString charToUtf8Hex(QChar ch);     // 将字符转换为 UTF-8 编码（十六进制）
    QString charToUtf16Hex(QChar ch);    // 将字符转换为 UTF-16 编码（十六进制）
    QString charToUnicodeHex(QChar ch);  // 将字符转换为 Unicode 编码（十六进制）
};

#endif // WIDGET_H
