#include "converter.h"
#include "ui_converter.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QStringConverter>

Converter::Converter(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , ui(new Ui::Converter)
    , parentWidget(parent)
{
    ui->setupUi(this);

    // 初始化编码选择下拉框
    ui->encodingComboBox->addItem("UTF-8");
    ui->encodingComboBox->addItem("GBK");

    // 连接信号槽
    connect(ui->selectFileButton, &QPushButton::clicked, this, &Converter::onSelectFileButtonClicked);
    connect(ui->convertButton, &QPushButton::clicked, this, &Converter::onConvertButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &Converter::onBackButtonClicked);

    setWindowTitle("文件编码转换器");
    ui->statusTextEdit->setPlaceholderText("请选择一个txt文件并选择目标编码...");
}

Converter::~Converter()
{
    delete ui;
}

void Converter::onSelectFileButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择文本文件",
        QDir::homePath(),
        "文本文件 (*.txt)"
        );

    if (!filePath.isEmpty()) {
        selectedFilePath = filePath;
        ui->statusTextEdit->append(QString("已选择文件: %1").arg(filePath));
    }
}

void Converter::onConvertButtonClicked()
{
    if (selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个文本文件！");
        return;
    }

    QString targetEncoding = ui->encodingComboBox->currentText();
    convertFile(selectedFilePath, targetEncoding);
}

void Converter::onBackButtonClicked()
{
    if (parentWidget) {
        parentWidget->show();
    }
    this->hide();
}

void Converter::convertFile(const QString &inputPath, const QString &encoding)
{
    // 读取源文件（自动检测编码）
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("无法打开文件: %1").arg(inputPath));
        return;
    }

    QTextStream in(&inputFile);
    QString content = in.readAll();
    inputFile.close();

    // 生成输出文件名
    QFileInfo fileInfo(inputPath);
    QString outputFileName = QString("%1-%2.txt").arg(fileInfo.baseName()).arg(encoding);
    QString outputPath = fileInfo.absolutePath() + "/" + outputFileName;

    // 写入目标文件
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", QString("无法创建输出文件: %1").arg(outputPath));
        return;
    }

    QTextStream out(&outputFile);

    // 设置输出编码
    if (encoding == "UTF-8") {
        out.setEncoding(QStringConverter::Utf8);
    }
    else if (encoding == "GBK") {
// Windows下使用本地编码（通常是GBK），其他平台需要额外处理
#ifdef Q_OS_WIN
        out.setEncoding(QStringConverter::System);
#else
        // 非Windows系统需要iconv或其他方式支持GBK
        QMessageBox::warning(this, "警告", "非Windows系统可能需要额外配置才能支持GBK编码");
        out.setEncoding(QStringConverter::System); // 回退到系统编码
#endif
    }

    // 直接写入QString，QTextStream会自动处理编码转换
    out << content;
    outputFile.close();

    ui->statusTextEdit->append(QString("转换完成！输出文件: %1").arg(outputPath));
    QMessageBox::information(this, "成功", QString("文件已成功转换为%1编码！\n输出文件: %2").arg(encoding).arg(outputPath));
}
