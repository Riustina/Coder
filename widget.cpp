#include "widget.h"
#include "ui_widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QScrollArea>
#include <QGridLayout>
#include <cstdlib>
#include <ctime>
#include "converter.h"
#include "clickablelabel.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , originalLayoutWidget(nullptr)
    , encodingLayoutWidget(nullptr)
    , converterWindow(nullptr)
{
    ui->setupUi(this);

    // 初始化随机数种子
    srand(time(nullptr));

    // 为容器创建网格布局，支持自动换行
    originalLayoutWidget = ui->originalTextContainer;
    QGridLayout *originalGridLayout = new QGridLayout(originalLayoutWidget);
    originalGridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    originalGridLayout->setSpacing(3);
    originalGridLayout->setContentsMargins(5, 5, 5, 5);

    encodingLayoutWidget = ui->encodingTextContainer;
    QGridLayout *encodingGridLayout = new QGridLayout(encodingLayoutWidget);
    encodingGridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    encodingGridLayout->setSpacing(3);
    encodingGridLayout->setContentsMargins(5, 5, 5, 5);

    // 连接按钮信号
    connect(ui->convertButton, &QPushButton::clicked, this, &Widget::onConvertButtonClicked);

    // 设置按钮文本
    ui->convertButton->setText("转换编码");
    ui->pushButton->setText("清空");

    // 连接清空按钮
    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        ui->textEdit->clear();
        clearContainerLayout(originalLayoutWidget);
        clearContainerLayout(encodingLayoutWidget);
        encodingMap.clear();
    });

    // 设置窗口标题
    setWindowTitle("文字编码显示器");

    // 设置文本编辑框的提示文本
    ui->textEdit->setPlaceholderText("请输入要转换的文本...");

    connect(ui->goConvertLabel, &ClickableLabel::clicked, this, &Widget::onLinkLabelClicked);
}

Widget::~Widget()
{
    delete ui;
    if (converterWindow) {
        delete converterWindow;
    }
}

void Widget::onLinkLabelClicked()
{
    if (!converterWindow) {
        converterWindow = new Converter(this);
    }
    converterWindow->show();
    this->hide();
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
    clearContainerLayout(originalLayoutWidget);
    clearContainerLayout(encodingLayoutWidget);

    // 获取布局
    QGridLayout *originalGridLayout = qobject_cast<QGridLayout*>(originalLayoutWidget->layout());
    QGridLayout *encodingGridLayout = qobject_cast<QGridLayout*>(encodingLayoutWidget->layout());

    // 计算每行能放多少个字符（基于容器宽度）
    int containerWidth = originalLayoutWidget->width();
    int itemWidth = 35; // 原文字符宽度
    int encodingItemWidth = 90; // 编码值宽度
    int spacing = 4;

    // 估算每行字符数（考虑滚动条等）
    int charsPerRow = std::max(1, (containerWidth - 20) / (itemWidth + spacing));
    int encodingPerRow = std::max(1, (containerWidth - 20) / (encodingItemWidth + spacing));

    // 为每个字符生成 Label 和编码信息
    for (int i = 0; i < inputText.length(); ++i) {
        QChar ch = inputText.at(i);

        // 生成编码信息
        EncodingInfo info;
        info.utf8Hex = charToUtf8Hex(ch);
        info.utf16Hex = charToUtf16Hex(ch);
        info.unicodeHex = charToUnicodeHex(ch);
        info.color = generateRandomColor();
        encodingMap[ch] = info;

        // 创建详细信息字符串
        QString tooltipText = QString("字符: %1\nUTF-8: %2\nUTF-16: %3\nUnicode: U+%4")
                                  .arg(ch)
                                  .arg(info.utf8Hex)
                                  .arg(info.utf16Hex)
                                  .arg(info.unicodeHex);

        // 原文显示：创建 Label
        QLabel *originalLabel = new QLabel(QString(ch));
        originalLabel->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; font-size: 14px;").arg(info.color));
        originalLabel->setAlignment(Qt::AlignCenter);
        originalLabel->setFixedSize(35, 35);
        originalLabel->setToolTip(tooltipText); // 添加工具提示

        // 计算网格位置
        int originalRow = i / charsPerRow;
        int originalCol = i % charsPerRow;
        originalGridLayout->addWidget(originalLabel, originalRow, originalCol);

        // 编码值显示：创建 Label
        QLabel *encodingLabel = new QLabel(info.utf8Hex);
        encodingLabel->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; font-size: 14px; font-family: monospace;").arg(info.color));
        encodingLabel->setAlignment(Qt::AlignCenter);
        encodingLabel->setFixedSize(90, 35);
        encodingLabel->setWordWrap(true);
        encodingLabel->setToolTip(tooltipText); // 添加工具提示

        // 计算编码值的网格位置
        int encodingRow = i / encodingPerRow;
        int encodingCol = i % encodingPerRow;
        encodingGridLayout->addWidget(encodingLabel, encodingRow, encodingCol);
    }

    qDebug() << "转换完成，处理了" << inputText.length() << "个字符";
}

void Widget::clearContainerLayout(QWidget *container)
{
    if (!container || !container->layout()) return;

    QLayoutItem *item;
    while ((item = container->layout()->takeAt(0)) != nullptr) {
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

QString Widget::charToUnicodeHex(QChar ch)
{
    uint32_t unicode = ch.unicode();
    return QString("%1").arg(unicode, 4, 16, QChar('0')).toUpper();
}
