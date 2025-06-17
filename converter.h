#ifndef CONVERTER_H
#define CONVERTER_H

#include <QWidget>
#include <QFileDialog>
#include <QStringConverter>

namespace Ui {
class Converter;
}

class Converter : public QWidget
{
    Q_OBJECT

public:
    explicit Converter(QWidget *parentWidget = nullptr);
    ~Converter();

private slots:
    void onSelectFileButtonClicked();
    void onConvertButtonClicked();
    void onBackButtonClicked();

private:
    Ui::Converter *ui;
    QString selectedFilePath;
    QWidget *parentWidget;

    void convertFile(const QString &inputPath, const QString &encoding);
};

#endif // CONVERTER_H
