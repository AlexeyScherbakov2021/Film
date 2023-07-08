#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QImage>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    isStop = false;
    ui->setupUi(this);
    ui->pushButtonStop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


struct LeftEdge
{
    QPoint pt;
};


void MainWindow::on_pushButton_clicked()
{
//    const int CADRS = 36;
    int startX;
    int y = 60;
    QString startPath = "d:\\Work\\QT\\Кинокамера\\0004";

    ui->pushButton->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);


    QDir dir(startPath);
    QFileInfoList listFiles = dir.entryInfoList(QStringList() << "*.jpg", QDir::Files);

    ui->progressBar->setRange(0, listFiles.count());
    ui->progressBar->setTextVisible(true);

    int step = 0;
    foreach (QFileInfo item, listFiles)
    {

        ui->progressBar->setValue(++step);
        if(isStop)
            break;

        img = new QImage(listFiles[292].absoluteFilePath());
//        img = new QImage(item.absoluteFilePath());
        LeftMargin = IsWhiteLine(img, 2, 42, y, y + 100);
        startX = LeftMargin > 0 ? LeftMargin + 2 : 0;
        if(LeftMargin < 0)
            startX = 0;


        int numberTape = 1;
        for(int x = startX; x < 4300 - 800 && numberTape <= 4; x += 780, numberTape++)
        {

            KinoTape *Tape = new KinoTape(img, item, numberTape);
            if(Tape->FindLeftBorder(x, y))
            {
                Tape->InitAllKadrs();
                Tape->FinalTuneKadrs();
                x = Tape->GetRightX();
            }
            QApplication::processEvents();
        }

        delete img;
    }

    isStop = true;
    ui->pushButton->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

}


//-----------------------------------------------------------------------------------------
// проверка пиксела на белый цвет
//-----------------------------------------------------------------------------------------
bool MainWindow::IsWhitePixel(QImage *img, int x, int y)
{
    QColor rgb = img->pixel(x, y);
    return (rgb.red() > 0xe0 && rgb.green() > 0xe0 && rgb.blue() > 0xe0);
}



//-----------------------------------------------------------------------------------------
// проверка линии на белый цвет
//-----------------------------------------------------------------------------------------
int MainWindow::IsWhiteLine(QImage *img, int x0, int x1, int y0, int y1)
{
    QColor rgb;
    double dY = (double)(y1 - y0) / (double)(x1 - x0);
    int countX = x1 - x0 + 1;
    int startWhite = -1;

    if(countX == 0)
        return -1;

    int y = y0;
//    int countWhite = 0;
    for(int x = x0; x < x1; x++)
    {
        rgb = img->pixel(x, y);
        if(rgb.red() > 0xc0 && rgb.green() > 0xc0 && rgb.blue() > 0xc0)
        {
            if(startWhite < 0)
                startWhite = x;
//            ++countWhite;
        }
        y =  (double)y + dY;
    }

    // находим процент белых пикселей
//    int res = countWhite * 100 / countX;

    return startWhite;

}


void MainWindow::on_pushButtonStop_clicked()
{
    isStop = true;
    ui->pushButton->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

}

