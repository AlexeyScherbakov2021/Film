#ifndef KINOTAPE_H
#define KINOTAPE_H

#include "ParamsKadr.h"
#include "kadr.h"

#include <QFileInfo>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QString>


enum TypeTape {Unknown, Super8, Standard};


class KinoTape
{
private:
    QList<Kadr*> listKadr;          // список кадров
    QList<QPoint*> leftBorder;      // список координат левой границы
    QImage* img;
    TypeTape type;
    const int DELTA_Y = 10;
    QString pathOutput;
    QString fileName;
    int number;

    bool GetFirstPerf();
    Kadr* FindRectPerf(int midX, int midY/*, int leftX*/);
    void CorrectLeftEdge(int startGoodY, /*int avgGoodX,*/ int countYconst);
//    bool TestBlackLeftMargin(int y);
    double CalcDeltaX(int fromY, int toY);
//    bool FindNextPerf(int& midX, int& midY);

public:
    ParamKadr* param;

    KinoTape(QImage *img, QFileInfo& path, int num);
    bool FindLeftBorder(int startX, int startY);
    int GetRightX();
//    QList<QPoint*> GetLeftBorder();

    int GetIndexY(int y);
    int GetLeftXFromY(int y);
    Kadr* FindNearKadr(Kadr* kadr, bool IsPrev);
    void InitAllKadrs();
    void FinalTuneKadrs();

    ~KinoTape();
};




//class KinoTapeS8 : KinoTape{

//};


//class KinoTape8 : KinoTape8{

//};

#endif // KINOTAPE_H
