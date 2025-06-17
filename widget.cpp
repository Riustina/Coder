#include "widget.h"
#include "ui_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QTextCodec>
#include <QScrollArea>
#include <cstdlib>
#include <ctime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , originalLayout(nullptr)
    , encodingLayout(nullptr)
{
    ui->setupUi(this);

    // 初始化随机数种子
    srand(time(nullptr));

    // 为容器创建水平布局
    originalLayout = new QHBoxLayout(ui->originalTextContainer);
    originalLayout->setAlignment(Qt::AlignLeft);
    originalLayout->setSpacing(2);
    originalLayout->setContentsMargins(5, 5, 5, 5);

    encodingLayout = new QHBoxLayout(ui->encodingTextContainer);
    encodingLayout->setAlignment(Qt::AlignLeft);
    encodingLayout->setSpacing(2);
    encodingLayout->setContentsMargins(5, 5, 5, 5);

    // 连接按钮信号
    connect(ui->convertButton, &QPushButton::clicked, this, &Widget::onConvertButtonClicked);

    // 设置按钮文本
    ui->convertButton->setText("转换编码");
    ui->pushButton->setText("清空");

    // 连接清空按钮
    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        ui->textEdit->clear();
        clearContainerLayout(originalLayout);
        clearContainerLayout(encodingLayout);
        encodingMap.clear();
    });

    // 设置窗口标题
    setWindowTitle("文字编码显示器");

    // 设置文本编辑框的提示文本
    ui->textEdit->setPlaceholderText("请输入要转换的文本...");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onConvertButtonClicked()
{
    qDebug() << "转换按钮被点击";

    // 获取输入文本
    QString inputText = ui->textEdit->toPlainText();

    if (inputText.isEmpty()) {
        qDebug() << "输入文本为空";
        return;
    }

    // 清空之前的内容
    encodingMap.clear();
    clearContainerLayout(originalLayout);
    clearContainerLayout(encodingLayout);

    // 为每个字符生成 Label 和编码信息
    for (int i = 0; i < inputText.length(); ++i) {
        QChar ch = inputText.at(i);

        // 生成编码信息
        EncodingInfo info;
        info.utf8Hex = charToUtf8Hex(ch);
        info.utf16Hex = charToUtf16Hex(ch);
        info.gbkHex = charToGbkHex(ch);
        info.color = generateRandomColor();
        encodingMap[ch] = info;

        // 原文显示：创建 Label
        QLabel *originalLabel = new QLabel(QString(ch));
        originalLabel->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; font-size: 14px; font-weight: bold;").arg(info.color));
        originalLabel->setAlignment(Qt::AlignCenter);
        originalLabel->setFixedSize(35, 35);
        originalLayout->addWidget(originalLabel);

        // 编码值显示：创建 Label（显示UTF-8编码）
        QLabel *encodingLabel = new QLabel(info.utf8Hex);
        encodingLabel->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; font-size: 10px; font-family: monospace;").arg(info.color));
        encodingLabel->setAlignment(Qt::AlignCenter);
        encodingLabel->setFixedSize(80, 35);
        encodingLabel->setWordWrap(true);
        encodingLabel->setToolTip(QString("字符: %1\nUTF-8: %2\nUTF-16: %3\nGBK: %4")
                                      .arg(ch)
                                      .arg(info.utf8Hex)
                                      .arg(info.utf16Hex)
                                      .arg(info.gbkHex));
        encodingLayout->addWidget(encodingLabel);
    }

    // 添加弹性空间，使标签左对齐
    originalLayout->addStretch();
    encodingLayout->addStretch();

    qDebug() << "转换完成，处理了" << inputText.length() << "个字符";
}

void Widget::clearContainerLayout(QHBoxLayout *layout)
{
    if (!layout) return;

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

QString Widget::generateRandomColor()
{
    // 生成较亮的颜色，确保文字可读性
    int r = 150 + rand() % 106;  // 150-255
    int g = 150 + rand() % 106;  // 150-255
    int b = 150 + rand() % 106;  // 150-255
    return QString("rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
}

QString Widget::charToUtf8Hex(QChar ch)
{
    QByteArray utf8 = QString(ch).toUtf8();
    QString hex = utf8.toHex().toUpper();

    // 格式化为更易读的形式
    QString formatted;
    for (int i = 0; i < hex.length(); i += 2) {
        if (i > 0) formatted += " ";
        formatted += hex.mid(i, 2);
    }
    return formatted;
}

QString Widget::charToUtf16Hex(QChar ch)
{
    uint16_t utf16 = ch.unicode();
    return QString("%1").arg(utf16, 4, 16, QChar('0')).toUpper();
}

QString Widget::charToGbkHex(QChar ch)
{
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    if (!gbkCodec) {
        return "N/A";
    }

    QByteArray gbkBytes = gbkCodec->fromUnicode(QString(ch));
    if (gbkBytes.isEmpty()) {
        return "N/A";
    }

    QString hex = gbkBytes.toHex().toUpper();
    QString formatted;
    for (int i = 0; i < hex.length(); i += 2) {
        if (i > 0) formatted += " ";
        formatted += hex.mid(i, 2);
    }
    return formatted;
}
