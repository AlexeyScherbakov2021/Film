#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "kinotape.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static bool IsWhitePixel(QImage *img, int x, int y);
    static int IsWhiteLine(QImage *img, int x0, int x1, int y0, int y1);

private slots:
    void on_pushButton_clicked();

    void on_pushButtonStop_clicked();

private:
    int LeftMargin;
    bool isStop;

    QImage *img;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
