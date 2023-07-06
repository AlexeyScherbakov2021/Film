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
//    QTransform trans;
    //trans.map(QPoint(100,0));
    //trans.translate(0, 200);
//    trans.rotate(Angle * 180 / 3.1415 );
//    QImage img2 = img1.transformed(trans);
    QString outPath = QString(dir + "\\%1_%2.jpg").arg(fileName.left(4)).arg(numKadr, 4, 10, QChar('0'));
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
//    float dAngle = atan((float)tape->param->YCenterFromPerf / (float)tape->param->XCenterFromPerf);
//    double dAngle = tape->param->YCenterFromPerf == 0
//        ? 0
//        : atan((double)tape->param->XCenterFromPerf / (double)tape->param->YCenterFromPerf);


    double dAngle = atan((double)tape->param->YCenterFromPerf / (double)tape->param->XCenterFromPerf);
    dAngle += Angle;        // общий угол на центр кадра от перфорации

    double gipot = sqrt((double)tape->param->YCenterFromPerf * (double)tape->param->YCenterFromPerf +
                        (double)tape->param->XCenterFromPerf * (double)tape->param->XCenterFromPerf);

    double dY = sin(dAngle) * gipot;
    double dX = cos(dAngle) * gipot;
    ptCenter.setX(ptPerf.x() + dX);
    ptCenter.setY(ptPerf.y() + dY);
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//void Kadr8::SaveKadr(QImage *img, int number, QString& fileName, QString& dir)
//{

//}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//float Kadr8::CalcAngle()
//{
//    int yTop = ptPerf.y() - param8.CenterPerfY;
//    int yBottom = ptPerf.y() + param8.Height - param8.CenterPerfY;

//    float xTop = tape->GetLeftXFromY(yTop);
//    float xBottom = tape->GetLeftXFromY(yBottom);

//    float ang = atan((xBottom - xTop) / -(yBottom - yTop));

//    return ang;
//}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//void Kadr8::SetCenterPoint()
//{

//}
