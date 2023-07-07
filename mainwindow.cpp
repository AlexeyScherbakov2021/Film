#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QImage>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    const int CADRS = 36;
    int startX;
    int y = 60;
    //QList<KinoTape*> listTape;
    QString startPath = "d:\\Work\\QT\\Кинокамера\\0003";
//    KinoTape *Tape[4];
//    QList<QPoint*> listPrevBorder;;

    QDir dir(startPath);
    QFileInfoList listFiles = dir.entryInfoList(QStringList() << "*.jpg", QDir::Files);

    foreach (QFileInfo item, listFiles)
    {
//        img = new QImage(listFiles[255].absoluteFilePath());
        img = new QImage(item.absoluteFilePath());
        LeftMargin = IsWhiteLine(img, 2, 42, y, y + 100);
        startX = LeftMargin > 0 ? LeftMargin + 2 : 0;
        if(LeftMargin < 0)
            startX = 0;

//        Tape[0] = nullptr;
//        Tape[1] = nullptr;
//        Tape[2] = nullptr;
//        Tape[3] = nullptr;
        //listPrevBorder = nullptr;

        int numberTape = 1;
        for(int x = startX; x < 4300 - 800 && numberTape <= 4; x += 780, numberTape++)
        {
//            Tape[numberTape] = new KinoTape(img, item, numberTape + 1);

            KinoTape *Tape = new KinoTape(img, item, numberTape);
//            KinoTape *Tape = new KinoTape(img, listFiles[48], numberTape + 1);
            if(Tape->FindLeftBorder(x, y))
            {
                Tape->InitAllKadrs();
                Tape->FinalTuneKadrs();
                //listTape.push_back(Tape);
//                listPrevBorder = Tape->GetLeftBorder();
                x = Tape->GetRightX();
            }
            //delete Tape;
        }

        delete img;
//        delete Tape[0];
//        delete Tape[1];
//        delete Tape[2];
//        delete Tape[3];
    }

}


//-----------------------------------------------------------------------------------------
// проверка пиксела на белый цвет
//-----------------------------------------------------------------------------------------
bool MainWindow::IsWhitePixel(QImage *img, int x, int y)
{
    QColor rgb = img->pixel(x, y);
    return (rgb.red() > 0xc0 && rgb.green() > 0xc0 && rgb.blue() > 0xc0);
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
    int countWhite = 0;
    for(int x = x0; x < x1; x++)
    {
        rgb = img->pixel(x, y);
        if(rgb.red() > 0xc0 && rgb.green() > 0xc0 && rgb.blue() > 0xc0)
        {
            if(startWhite < 0)
                startWhite = x;
            ++countWhite;
        }
        y =  (double)y + dY;
    }

    // находим процент белых пикселей
    int res = countWhite * 100 / countX;

    return startWhite;

}




//bool GetTypeLine(QImage &img, int startX, int startY)
//{

//    for(int y = startY; y < img.height() - 90; y += 10)
//    {
//        for(int i = startX; i < startX + 94; i++)
//        {
//            QColor rgb = img.pixel(i, y);

//            if(rgb.red() > 0xdd && rgb.green() > 0xdd && rgb.blue() > 0xdd)
//            {
//                rgb = img.pixel(i + 4, y);
//                if(rgb.red() > 0xdd && rgb.green() > 0xdd && rgb.blue() > 0xdd)
//                    goto mFind;
//            }
//        }

//    }

//    return false;


//    mFind:

//    return true;

//}


//void SetVerticalLenta(QList<LeftEdge*> listEdge)
//{
//    for(int i = 1; i < listEdge.size() - 1; i++)
//    {
//        if(listEdge[i]->pt.x() - listEdge[i-1]->pt.x() > 10
//            && listEdge[i+1]->pt.x() - listEdge[i]->pt.x() > 10)
//            listEdge[i].pt.setX((listEdge[i-1]->pt.x() + listEdge[i+1]->pt.x())/2);


//    }
//}
