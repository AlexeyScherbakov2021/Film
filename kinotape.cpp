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
    int currX;
    int y = startY;
    int countYconst = 0;
    int oldX = -1;
    int startGoodY = 0;
    bool isEndCount = false;

    int endX;
    QPoint *pt;

    while(y < img->height() - 50)
    {
        pt = new QPoint(-1, y);
        currX = -1;

        if(startX < 0)
            startX = 0;

        if(MainWindow::IsWhitePixel(img, startX, y))
        {
            // белый пиксель, будем двигаться вправо
            endX =  (startX + countStep) < (img->width() - 300) && y > startY ? startX + countStep : img->width() - 300;
            ++startX;
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
                        // правее, тоже не белые. Подходит
                        pt->setX(x);
                        currX = x;
                        break;
                    }
                }
            }
        }

        else
        {
            // не белый пиксель, будем двигаться влево
            endX = (startX - countStep) > 0 ? startX - countStep : 0;
            --startX;
            for(int x = startX; x >= endX; x--)
            {
                if(MainWindow::IsWhitePixel(img, x, y))
                {
                    // белый пиксель найден

                    if(x < 15 || (MainWindow::IsWhitePixel(img, x - 2, y + 4) && MainWindow::IsWhitePixel(img, x - 4, y + 6)))
                    {
                        // левее, тоже белые. Подходит
                        ++x;
                        pt->setX(x);
                        currX = x;
                        break;
                    }
                }
            }

        }

        leftBorder.push_back(pt);


        if( pt->x() > 0 && ( abs(pt->x() - oldX) < 6 || oldX == -1))
        {
            // было малое отклонение
            if(startGoodY == 0)
            {
                // если счетчика не было, обнуляем
                startGoodY = y;
                countYconst = 0;
            }

            if(!isEndCount)
                ++countYconst;

        }
        else if(countYconst < 100)
        {
            startGoodY = 0;
        }
        else
            isEndCount = true;


        oldX = currX;;
        startX = currX;
        y += DELTA_Y;

    }

    if(countYconst > 0)
    {
        CorrectLeftEdge(startGoodY, /*avgGoodX,*/ countYconst);
        return true;
    }

    return false;
}


//------------------------------------------------------------------------------------------------
// проверка на левый черный отступ
//------------------------------------------------------------------------------------------------
bool KinoTape::TestBlackLeftMargin(int y)
{
    return true;
//    LeftMargin = MainWindow::IsWhiteLine(img, 2, 42, y, y + 100);

}



//------------------------------------------------------------------------------------------------
// корректировка левой границы
//------------------------------------------------------------------------------------------------
void KinoTape::CorrectLeftEdge(int startGoodY, int countYconst)
{
    double dX;
    double CurrX;
    int index = GetIndexY(startGoodY);
    int lastCorrIndex = index + countYconst - 1;
    const int cntCalcY = countYconst > 50 ? 50 : countYconst;

    if(index > 0)
    {
        while(index > 0)
        {
            // поиск следуюшего верного индекса
            int lastInd = -1;
            int cntCorrect = 0;

            for(int ind = index - 1; ind > 0; ind--)
            {
                int deltaX = abs(leftBorder[ind]->x() - leftBorder[ind - 1]->x());
                if(deltaX < 6 && leftBorder[ind]->x() != -1)
                {
                    if( ++cntCorrect > 4 && abs(leftBorder[index]->x() - leftBorder[ind]->x()) < 20)
                    {
                        lastInd = ind + cntCorrect - 1;
                        break;
                    }
                }
                else
                {
                    cntCorrect = 0;
                }
            }


            if(lastInd != -1)
            {


                // корректировка X участка
                dX = (double)(leftBorder[lastInd]->x() - leftBorder[index]->x()) / (double)(lastInd - index);
                CurrX = leftBorder[index]->x();
                for(int ind = index; ind > lastInd; ind--)
                {
                    CurrX += dX;
                    leftBorder[ind]->setX(CurrX);
                }

                // поиск следующего плохого участка
                for(; lastInd > 0; lastInd--)
                {
                    if( leftBorder[lastInd]->x() - leftBorder[lastInd - 1]->x() > 6)
                    {
                        //lastInd = ind;
                        break;
                    }
                }


            }
            else
            {
                // до конца не было коректных значений
                // вычисление среднего отклонения X
                dX = CalcDeltaX(index, index + cntCalcY);

                // корректировка вверх
                CurrX = leftBorder[index]->x();

                for(int ind = index - 1; ind >= 0; ind--)
                {
                    CurrX += dX;
                    leftBorder[ind]->setX(CurrX);
                }
                break;
            }
            index = lastInd;
        }
    }



    // корректировка вниз
    while(lastCorrIndex < leftBorder.count())
    {
        // поиск следуюшего верного индекса
        int lastInd = -1;
        int cntCorrect = 0;

        for(int ind = lastCorrIndex + 1; ind < leftBorder.count() - 1; ind++)
        {
            int deltaX = abs(leftBorder[ind]->x() - leftBorder[ind + 1]->x());
            if(deltaX < 6 && leftBorder[ind]->x() != -1)
            {
                if( ++cntCorrect > 4 && abs(leftBorder[lastCorrIndex]->x() - leftBorder[ind]->x()) < 20)
                {
                    lastInd = ind - cntCorrect + 1;
                    break;
                }
            }
            else
            {
                cntCorrect = 0;
            }
        }



        if(lastInd != -1)
        {
            // корректировка X участка
            dX = (double)(leftBorder[lastInd]->x() - leftBorder[lastCorrIndex]->x()) / (double)(lastInd - lastCorrIndex);
            //dX *= DELTA_Y;
            CurrX = leftBorder[lastCorrIndex]->x();
            for(int ind = lastCorrIndex; ind < lastInd; ind++)
            {
                CurrX += dX;
                leftBorder[ind]->setX(CurrX);
            }

            // поиск следующего плохого участка
            for(; lastInd < leftBorder.count() - 1; lastInd++)
            {
                if( leftBorder[lastInd]->x() - leftBorder[lastInd + 1]->x() > 6)
                {
                    //lastInd = ind;
                    break;
                }
            }

        }
        else
        {
            // до конца не было коректных значений
            dX = CalcDeltaX(lastCorrIndex - cntCalcY, lastCorrIndex - 1);
            CurrX = leftBorder[lastCorrIndex - 1]->x();
            //++lastCorrIndex;
            for(int ind = lastCorrIndex; ind < leftBorder.count(); ind++)
            {
                CurrX += dX;
                leftBorder[ind]->setX(CurrX);
            }
            break;
        }

        lastCorrIndex = lastInd;

    }

}

//------------------------------------------------------------------------------------------------
// расчет отклонения Х на указаном отрезке Y
//------------------------------------------------------------------------------------------------
double KinoTape::CalcDeltaX(int fromY, int toY)
{
    if(fromY < 0)
        fromY = 0;

    if(toY >=  leftBorder.count())
        toY = leftBorder.count() - 1;

    double summa = 0;

    int cntCalcY = toY - fromY;
    for(int n = fromY; n < toY; n++, cntCalcY++)
        summa += (double)(leftBorder[n + 1]->x() - leftBorder[n]->x());

    double dX = summa / cntCalcY;
    return dX;

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
    const int dX = 120;
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








