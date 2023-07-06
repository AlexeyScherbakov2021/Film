#include "kinotape.h"
#include "mainwindow.h"
#include <QDir>
#include <math.h>


ParamKadrSuper8 paramS8;
ParamKadr8 param8;

KinoTape::KinoTape(QImage* imgIn, QFileInfo& fileInfo, int num)
{
    type = TypeTape::Unknown;
    number = num;
    fileName = fileInfo.fileName();
    pathOutput = fileInfo.absolutePath();
    img = imgIn;
    //img = new QImage(fileInfo.absoluteFilePath());
}

//------------------------------------------------------------------------------------------------
// получение левой границы пленки
//------------------------------------------------------------------------------------------------
bool KinoTape::FindLeftBorder(int startX, int startY)
{
    const int DELTA_Y = 10;
    const int countStep = 280;

    int y = startY;
    int countYconst = 0;
    int oldX = -1;
    bool isBegin;
    int startGoodY = 0;
    int avgGoodX = 0;
    int sumX = 0;
    int x;

    int endX;
    QPoint *pt;

    while(y < img->height())
    {
        pt = new QPoint(-1, y);

        if(MainWindow::IsWhitePixel(img, x, y))
        {
            // белый пиксель, будем двигаться вправо
            endX =  (startX + countStep) < (img->width() - 300) ? startX + countStep : img->width() - 300;
            for(int x = startX; x < endX; x++)
            {

                if(!MainWindow::IsWhitePixel(img, x, y))
                {
                    // не белый пиксель найден

                    if(!MainWindow::IsWhitePixel(img, x + 10, y)
                        && !MainWindow::IsWhitePixel(img, x + 15, y)
                        && !MainWindow::IsWhitePixel(img, x + 20, y)
                        && !MainWindow::IsWhitePixel(img, x + 25, y)
                        && !MainWindow::IsWhitePixel(img, x + 30, y))
                    {
                        // првее, тоже не белые. Подходит
                        pt->setX(x);
                        break;
                    }
                }
            }
        }

        else
        {
            // не белый пиксель, будем двигаться влево
            endX = (startX - countStep) > 0 ? startX - countStep : 0;
            for(int x = startX; x > endX; x--)
            {
                if(MainWindow::IsWhitePixel(img, x, y))
                {
                    // белый пиксель найден

                    if(!MainWindow::IsWhitePixel(img, x - 10, y) && !MainWindow::IsWhitePixel(img, x - 15, y))
                    {
                        // левее, тоже белые. Подходит
                        --x;
                        pt->setX(x);
                        break;
                    }
                }
            }

        }

        leftBorder.push_back(pt);
        startX = x;
        y += DELTA_Y;

    }




//    while(y < img->height())
//    {
//        isBegin = true;

//        for(int x = startX; x < img->width() - 300; x++)
//        {
//            if(!MainWindow::IsWhitePixel(img, x, y)             // если не белые пиксели по длине от 0 до 30
//                && !MainWindow::IsWhitePixel(img, x + 10, y)
//                && !MainWindow::IsWhitePixel(img, x + 15, y)
//                && !MainWindow::IsWhitePixel(img, x + 20, y)
//                && !MainWindow::IsWhitePixel(img, x + 25, y)
//                && !MainWindow::IsWhitePixel(img, x + 30, y))
//            {
//                if(isBegin)
//                {
//                    // был первым черный пиксель, перемещение назад
//                    x -= 5;
//                    if(x < 0)
//                        break;

//                    if(x < startX)
//                    {
//                        startX = x;
//                        continue;
//                    }

//                    continue;
//                }


//                if( abs(x - oldX) < 10 || oldX == -1)        // если разница с предыдущей точкой не более 5 пикселов
//                {
////                    if(countYconst == 0 && oldX != -1)      // если до этой точки были пропуски
////                    {
////                        int current = leftBorder.count();                       // текущий индекс
////                        float dX = (float)(x - oldX) / (float)(current - oldIndex) /** (float)DELTA_Y*/;       // приращение Х
////                        float x1 = (float)oldX + dX;                                            // первая точк аХ
////                        for(int index = oldIndex + 1; index < current; index++, x1 += dX) // коррекция координат Х
////                        {
////                            leftBorder[index]->setX(x1);
////                        }
////                    }

//                    if(countYconst == 0 && avgGoodX == 0)
//                        startGoodY = y;

//                    ++countYconst;
//                    sumX += x;

//                    if(countYconst >= 100)
//                    {
//                        avgGoodX = sumX / countYconst;
//                    }


//                    oldX = x;
//                    //oldIndex = leftBorder.count();

//                    if(x < startX)
//                        startX = x;

////                    startX = x >= 20 ? x - 20 : 0;
//                    //QPoint *pt = new QPoint( x, y);
//                    leftBorder.push_back(new QPoint( x, y));
//                }
//                else                                        // разница большая, делаем пропуск
//                {
//                    oldX = x;

//                    if(avgGoodX == 0)
//                    {
//                        startGoodY = -1;
//                        countYconst = 0;
//                        avgGoodX = 0;
//                        sumX = 0;
//                    }

//                    leftBorder.push_back(new QPoint( x, y));
////                    pt->setX(-1);
//                }

//                break;
//            }
//            isBegin = false;
//        }
//        y += DELTA_Y;
//    }

//    if(countYconst > 0)
//    {
//        CorrectLeftEdge(startGoodY, avgGoodX, countYconst);
//        return true;
//    }

    return true;
}


//------------------------------------------------------------------------------------------------
// проверка на левый черный отступ
//------------------------------------------------------------------------------------------------
bool KinoTape::TestBlackLeftMargin(int y)
{
//    LeftMargin = MainWindow::IsWhiteLine(img, 2, 42, y, y + 100);

}



//------------------------------------------------------------------------------------------------
// корректировка левой границы
//------------------------------------------------------------------------------------------------
void KinoTape::CorrectLeftEdge(int startGoodY, int avgGoodX, int countYconst)
{
    int index = GetIndexY(startGoodY);


    if(index > 0)
    {
        // вычисление среднего отклонения X
        double summa = 0;
        for(int n = index; n < index + 4; n++)
        {
            summa += (double)(leftBorder[n + 1]->x() - leftBorder[n]->x()) / (double)(leftBorder[n + 1]->y() - leftBorder[n]->y());
        }

        double dX = summa / 4;

        // корректировка вверх
        double CurrX = leftBorder[index]->x();

        for(int ind = index - 1; ind >= 0; ind--)
        {
            CurrX += dX;
            leftBorder[ind]->setX(CurrX);
        }
    }



    // корректировка вниз
    index += countYconst + 1;


//    int oldX = leftBorder[index]->x();

//    for(int ind = index + 100; ind < leftBorder.count(); ind++)
//    {

    //    }
}



//------------------------------------------------------------------------------------------------
// получение левой X левой верхней точки
//------------------------------------------------------------------------------------------------
int KinoTape::GetRightX()
{


    return leftBorder.empty() ? -1 : leftBorder[0]->x();
}

//------------------------------------------------------------------------------------------------
// получение левой X левой верхней точки
//------------------------------------------------------------------------------------------------
//QList<QPoint*> KinoTape::GetLeftBorder()
//{

//    return leftBorder;
//}

//------------------------------------------------------------------------------------------------
// получение первой перфорации и типа пленки
//------------------------------------------------------------------------------------------------
bool KinoTape::GetFirstPerf()
{
    const int dX = 150;
    int findX;
    int findIndex;
    int y = 0;
    int startIndex = GetIndexY(250);

    for(int index = startIndex; index < leftBorder.count(); index++)
    {
        for(int x = 45; x < dX; x++)
        {
            // если найден белый пиксел
            if(MainWindow::IsWhitePixel(img, leftBorder[index]->x() + x, leftBorder[index]->y()))
            {
                // если вни и вверх на 10 пикселов тоже белые
                if(MainWindow::IsWhitePixel(img, leftBorder[index]->x() + x, leftBorder[index]->y() + 10) ||
                    MainWindow::IsWhitePixel(img, leftBorder[index]->x() + x, leftBorder[index]->y() - 10))
                {
                    findX = leftBorder[index]->x() + x;
                    findIndex = index;
                    y = leftBorder[findIndex]->y();
                    // ищем первую перфорацию
                    Kadr *kadr = FindRectPerf(findX, y);
                    // если найдена
                    if(kadr != nullptr)
                    {
//                        kadr->ptPerf.setX(findX);
//                        kadr->ptPerf.setY(y);
//                        kadr->Angle = kadr->CalcAngle();
                        index = type == TypeTape::Super8 ? (y - param->Height/2) / param->Height : (y - (param->Height - param->HeightPerf/2)) / param->Height;

                        while(index--)
                        {
                            Kadr *kadrPrev = kadr;
                            kadrPrev = FindNearKadr(kadrPrev, true);
                            if(kadrPrev != nullptr)
                                listKadr.push_front(kadrPrev);
                        }

                        listKadr.push_back(kadr);
                        return true;
                    }
                }

                y += 350;
                index = GetIndexY(y) - 1;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------------------------
// нахождение средней точки перфорации
//------------------------------------------------------------------------------------------------
Kadr* KinoTape::FindRectPerf(int midX, int midY)
{
    int leftX = -1;
    int rightX = -1;
    int topY = -1;
    int bottomY = -1;
    int resX = midX;
    int resY = midY;

    Kadr* kadr = nullptr;

    int leftEdge = GetLeftXFromY(midY);
    int step = 3;

    while(step > 0)
    {
        --step;
        // выявление левой границы
        for(int x = resX; x > leftEdge; x--)
        {
            if(!MainWindow::IsWhitePixel(img, x, resY))
            {
                leftX = x;
                break;
            }
        }


        // выявление правой границы
        for(int x = resX; x < leftEdge + 270; x++)
        {
            if(!MainWindow::IsWhitePixel(img, x, resY))
            {
                rightX = x;
                break;
            }
        }

        // средння точка
        resX = leftX + (rightX - leftX) / 2;

        // выявление верхней границы
        for(int y = midY; y > resY - 122; y--)
        {
            if(!MainWindow::IsWhitePixel(img, resX, y))
            {
                topY = y;
                break;
            }
        }

        // выявление нижней границы
        for(int y = resY; y < topY + 150; y++)
        {
            if(!MainWindow::IsWhitePixel(img, resX, y))
            {
                bottomY = y;
                break;
            }
        }

        resY = topY + (bottomY - topY)/2;
    }

    if( (bottomY - topY) > 100 && (bottomY - topY) < 115
        && (rightX - leftX) > 70 &&  (rightX - leftX) < 92)
    {
        kadr = new KadrSuper8(this);
        type = TypeTape::Super8;
        param = &paramS8;
    }

    else if( (bottomY - topY) > 110 && (bottomY - topY) < 128
        && (rightX - leftX) > 160 &&  (rightX - leftX) < 178)
    {
        kadr = new Kadr8(this);
        type = TypeTape::Standard;
        param = &param8;
    }
    else
        return nullptr;

    kadr->ptPerf.setX(resX);
    kadr->ptPerf.setY(resY);
    kadr->Angle = kadr->CalcAngle();
    return kadr;

}


//------------------------------------------------------------------------------------------------
// нахождение средней точки для известного типа кадра
//------------------------------------------------------------------------------------------------
//bool KinoTape::FindNextPerf(int& midX, int& midY)
//{
//    int leftX = -1;
//    int rightX = -1;
//    int topY = -1;
//    int bottomY = -1;
//    int resX, resY;

//    int leftEdge = GetLeftXFromY(midY);
//    int step = 3;

//    while(step > 0)
//    {
//        --step;
//        // выявление левой границы
//        for(int x = midX; x > leftEdge; x--)
//        {
//            if(!MainWindow::IsWhitePixel(img, x, midY))
//            {
//                leftX = x;
//                break;
//            }
//        }


//        // выявление правой границы
//        for(int x = midX; x < leftX + param->WidthPerf + 10; x++)
//        {
//            if(!MainWindow::IsWhitePixel(img, x, midY))
//            {
//                rightX = x;
//                break;
//            }
//        }

//        // средння точка
//        resX = leftX + (rightX - leftX) / 2;

//        // выявление верхней границы
//        for(int y = midY; y > midY - (param->HeightPerf/2 + 10); y--)
//        {
//            if(!MainWindow::IsWhitePixel(img, resX, y))
//            {
//                topY = y;
//                break;
//            }
//        }

//        // выявление нижней границы
//        for(int y = midY; y < topY + param->HeightPerf + 10; y++)
//        {
//            if(!MainWindow::IsWhitePixel(img, resX, y))
//            {
//                bottomY = y;
//                break;
//            }
//        }

//        resY = topY + (bottomY - topY)/2;

//        if( (bottomY - topY) > param->WidthPerf - 10 && (bottomY - topY) < param->HeightPerf + 10
//            && (rightX - leftX) > param->WidthPerf - 10 &&  (rightX - leftX) < param->WidthPerf + 10)
//        {
//            midX = resX;
//            midY = resY;
//            return true;
//        }

//    }

//    return false;

//}


//------------------------------------------------------------------------------------------------
// поиск индекса до ближайшей y
//------------------------------------------------------------------------------------------------
int KinoTape::GetIndexY(int y)
{
    for(int index = 0; index < leftBorder.count() - 1; index++  )
    {
        if(y - leftBorder[index]->y() < DELTA_Y)
        {
            if(leftBorder[index + 1]->y() - y < y - leftBorder[index]->y())
                return index + 1;

            return index;
        }
    }

    return leftBorder.count() - 1;
}

//------------------------------------------------------------------------------------------------
// вычисление точки перфорации для соседнего кадра
//------------------------------------------------------------------------------------------------
Kadr* KinoTape::FindNearKadr(Kadr* kadr, bool IsPrev)
{
    int direct = IsPrev ? -1 : 1;

    int y1 = kadr->ptPerf.y() + param->Height * direct;  // расчетный Y для следующего кадра
    if(y1 < 0 || y1 >= img->height())
        return nullptr;

    int x1 = GetLeftXFromY(y1) + param->CenterPerfX;    // расчетный X для следующего кадра

    Kadr* newKadr;
    if(type == TypeTape::Super8)
        newKadr = new KadrSuper8(this);
    else
        newKadr = new Kadr8(this);

    // приращение координат
    float dX = x1 - kadr->ptPerf.x();
    float dY = y1 - kadr->ptPerf.y();

    // наклон пленки на промежутке кадра
    float Angl = atan(dX / -dY);

    int newX = kadr->ptPerf.x() - sin(Angl) * param->Height;
    int newY = kadr->ptPerf.y() + cos(Angl) * param->Height * direct;

    newKadr->ptPerf.setX(newX);
    newKadr->ptPerf.setY(newY);
    newKadr->Angle = Angl;

    return newKadr;
}


//------------------------------------------------------------------------------------------------
// поиск X левой границы по Y
//------------------------------------------------------------------------------------------------
int KinoTape::GetLeftXFromY(int y)
{
    int ind = GetIndexY(y);
    return leftBorder[ind]->x();
}

//------------------------------------------------------------------------------------------------
// проход по всей пленке и инициализация кадров
//------------------------------------------------------------------------------------------------
void KinoTape::InitAllKadrs()
{
    // поисе первой перфорации, определение типа пленки
    GetFirstPerf();

    Kadr* oldKadr = listKadr.back();
    int endY = img->height() - (param->Height - param->CenterPerfY);

    // добавление следующих кадров
    int x = oldKadr->ptPerf.x();
    for(int y = oldKadr->ptPerf.y() + param->Height; y < endY; y += param->Height)
    {
        Kadr* kadr = FindRectPerf(x, y);
        if(kadr == nullptr)
        {
            kadr = FindNearKadr(oldKadr, false);
        }
        if(kadr != nullptr)
        {
            x = kadr->ptPerf.x();
            y = kadr->ptPerf.y();
            listKadr.push_back(kadr);
            oldKadr = kadr;
        }
    }
}


//------------------------------------------------------------------------------------------------
// Финальная обработка кадров
//------------------------------------------------------------------------------------------------
void KinoTape::FinalTuneKadrs()
{
    QString outDir = QString(pathOutput + "/%1").arg(number, 4, 10, QChar('0'));

    if(!QDir(outDir).exists())
        QDir().mkdir(outDir);

    int i = 1;
    foreach (Kadr* kadr, listKadr)
    {
        kadr->SetCenterPoint();
        kadr->SaveKadr(img, i++, fileName, outDir);
    }
}

//------------------------------------------------------------------------------------------------
// деструктор
//------------------------------------------------------------------------------------------------
KinoTape::~KinoTape()
{
    //delete img;
    foreach (Kadr* item, listKadr)
        delete item;

    foreach (QPoint* item, leftBorder)
    {
        delete item;
    }
}








