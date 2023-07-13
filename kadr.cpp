#include "kinotape.h"
#include "kadr.h"

#include <math.h>

//----------------------------------------------------------------------------------
// запись кадра в файл
//----------------------------------------------------------------------------------
void Kadr::SaveKadr(QImage *img, int numKadr, QString& fileName, QString& dir)
{
    QRect rect(
        ptCenter.x() - tape->param->HalfKadrX,
        ptCenter.y() - tape->param->HalfKadrY,
        tape->param->HalfKadrX * 2,
        tape->param->HalfKadrY * 2
        );

    QImage img1 = img->copy(rect);
    QString outPath = QString(dir + "/%1_%2_%3.jpg").arg(dir.right(1)).arg(fileName.left(4)).arg(numKadr, 4, 10, QChar('0'));

//    if(abs(Angle) > 0.001)
//    {
//        QTransform trans;
//        trans.map(QPoint(100,0));
//        trans.translate(0, 200);
//        trans.rotate(-Angle * 180 / 3.1415926535 );
//        QImage img2 = img1.transformed(trans);
//        img2.save(outPath);

//    }
//    else
        img1.save(outPath);

}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
double Kadr::CalcAngle()
{
    int yTop = ptPerf.y() - tape->param->CenterPerfY;
    int yBottom = ptPerf.y() + (tape->param->Height - tape->param->CenterPerfY);

    double xTop = tape->GetLeftXFromY(yTop);
    double xBottom = tape->GetLeftXFromY(yBottom);

    double ang = atan((xBottom - xTop) / -(yBottom - yTop) );

    return ang;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
void Kadr::SetCenterPoint()
{

    double dAngle = atan((double)tape->param->YCenterFromPerf / (double)tape->param->XCenterFromPerf);
    dAngle += Angle;        // общий угол на центр кадра от перфорации

    double gipot = sqrt((double)tape->param->YCenterFromPerf * (double)tape->param->YCenterFromPerf +
                        (double)tape->param->XCenterFromPerf * (double)tape->param->XCenterFromPerf);

    double dY = sin(dAngle) * gipot;
    double dX = cos(dAngle) * gipot;
    ptCenter.setX(ptPerf.x() + dX);
    ptCenter.setY(ptPerf.y() + dY);
}

