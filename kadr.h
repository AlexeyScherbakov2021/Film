#ifndef KADR_H
#define KADR_H


#include <QImage>
#include "ParamsKadr.h"

extern ParamKadrSuper8 paramS8;
extern ParamKadr8 param8;


class KinoTape;

class Kadr {
protected:
    KinoTape *tape;

public:
    QPoint ptPerf;
    QString nameKadr;
    double Angle;
    QPoint ptCenter;

    Kadr(KinoTape *t) : tape(t) {  }
    virtual ~Kadr() {}
    virtual double CalcAngle();
    virtual void SetCenterPoint();
    virtual void SaveKadr(QImage* img, int number, QString& fileName, QString& dir);
};


class KadrSuper8 : public Kadr
{

public:
    KadrSuper8(KinoTape *t): Kadr(t) {}
    ~KadrSuper8() {}

};


class Kadr8 : public Kadr
{
public:
    Kadr8(KinoTape *t) : Kadr(t) {}
    ~Kadr8() {}

};

#endif // KADR_H
