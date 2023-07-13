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
    double CalcDeltaX(int fromY, int toY);
    bool GetBlackPixelLine(int x0, int y0, int x1, int y1, QPoint *pt);
    bool RectPerf(int& midX, int& midY, QRect *rc = nullptr);
    int SetStabilValue(QList<int>& list);

public:
    ParamKadr* param;

    KinoTape(QImage *img, QFileInfo& path, int num);
    bool FindLeftBorder(int startX, int startY);
    int GetRightX();

    int GetIndexY(int y);
    int GetLeftXFromY(int y);
    Kadr* FindNearKadr(Kadr* kadr, bool IsPrev);
    void InitAllKadrs();
    void FinalTuneKadrs();

    ~KinoTape();
};



#endif // KINOTAPE_H
