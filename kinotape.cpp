#include "kinotape.h"
#include "mainwindow.h"
#include <QDir>
#include <QList>
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
    int LeftEdgeX = startX;
    int widthX;
    bool isFirst = true;

    int endX;
    QPoint *pt;

    startX = -1;
    while(y < img->height() - 50)
    {
        pt = new QPoint(-1, y);
        currX = -1;

        widthX = countStep;
        if(startX < 0)
        {
            startX = LeftEdgeX;
            widthX = img->width();

        }

        if(MainWindow::IsWhitePixel(img, startX, y) || (isFirst && startX > 0 ))
        {

            // белый пиксель, будем двигаться вправо

            endX =  (startX + widthX) < (img->width() - 300) && y > startY ? startX + widthX : img->width() - 300;
            ++startX;
            for(int x = startX; x < endX; x++)
            {

                if(!MainWindow::IsWhitePixel(img, x, y))
                {
                    if(isFirst && x - startX < 20)
                        continue;

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
            if(startX < 0)
                pt->setX(0);
        }

        isFirst = false;
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


        if(currX >= 0)
        {
            oldX = currX;
        }
        startX = currX;
        y += DELTA_Y;

    }

    if(countYconst >= 100)
    {
        CorrectLeftEdge(startGoodY, countYconst);
        return true;
    }

    return false;
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
                // если они и вверх на 10 пикселов тоже белые
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

    midX = resX;
    midY = resY;

    QRect rc;
    RectPerf(midX, midY, &rc);

    if( (rc.bottom() - rc.top()) > 100 && (rc.bottom() - rc.top()) < 115
        && (rc.right() - rc.left()) > 67 &&  (rc.right() - rc.left()) < 92)
    {
        kadr = new KadrSuper8(this);
        type = TypeTape::Super8;
        param = &paramS8;
    }

    else if( (rc.bottom() - rc.top()) > 110 && (rc.bottom() - rc.top()) < 128
        && (rc.right() - rc.left()) > 160 &&  (rc.right() - rc.left()) < 178)
    {
        kadr = new Kadr8(this);
        type = TypeTape::Standard;
        param = &param8;
    }
    else
        return nullptr;

    kadr->ptPerf.setX(midX);
    kadr->ptPerf.setY(midY);
    kadr->Angle = kadr->CalcAngle();
    return kadr;

}


//------------------------------------------------------------------------------------------------
// нахождение темной точки по нправлении линии
//------------------------------------------------------------------------------------------------
bool KinoTape::GetBlackPixelLine(int x0, int y0, int x1, int y1, QPoint *pt)
{

    int x = x0;
    int y = y0;
    int minX = x0;
    int maxX = x1;
    int minY = y0;
    int maxY = y1;

    for(;;)
    {
        if(!MainWindow::IsWhitePixel(img, x, y))
        {
            maxX = x;
            maxY = y;
            // найден черный пиксель
            pt->setX(x);
            pt->setY(y);

            if( (abs(maxX - minX) <= 1 && abs(maxY - minY) <= 1) || (x == x0 && y == y0))
            {
                pt->setX(x);
                pt->setY(y);
                break;
            }

            x -= (maxX - minX) / 2;
            y -= (maxY - minY) / 2;
            continue;

        }

        minX = x;
        minY = y;
        x += (maxX - minX) / 2;
        y += (maxY - minY) / 2;

        if(abs(maxX - minX) <= 1 && abs(maxY - minY) <= 1)
            break;
    }

    // проверить часть линии до конца на темный цвет

//    while( x1 - x >= 6 || y1 - y >= 6)
//    {
//        x1 = x + (x1 - x) /2;
//        y1 = y + (y1 - y) /2;
//    }

//    if(MainWindow::IsWhitePixel(img, x1, y1))
//        return false;

//    x1 = x + (x1 - x) /2;
//    y1 = y + (y1 - y) /2;

//    if(MainWindow::IsWhitePixel(img, x1, y1))
//        return false;

    return true;
}


//------------------------------------------------------------------------------------------------
// удалание из списка выделяющихся значений
//------------------------------------------------------------------------------------------------
int KinoTape::SetStabilValue(QList<int>& list)
{
    int summa = 0;
    int avg;
    int minVal = 2000000;
    int maxVal = -1;
    int mid;

    foreach (int n, list)
    {
        summa += n;
        if(n > maxVal) maxVal = n;
        if(n < minVal) minVal = n;
    }

    avg = summa / list.count();
    mid = (maxVal - minVal) / 2;

    summa = 0;
    for(int i = 0; i < list.count(); ++i)
    {
        if(abs(list[i] - avg) > mid)
        {
            list.removeAt(i);
            --i;
            continue;
        }
        summa += list[i];
    }

    return summa / list.count();
}

//------------------------------------------------------------------------------------------------
// нахождение средней точки для известного типа кадра
//------------------------------------------------------------------------------------------------
bool KinoTape::RectPerf(int& midX, int& midY, QRect *rc)
{
    int widthPerf;
    int heightPerf;
    QPoint pt;

    if(type == TypeTape::Unknown)
    {
        widthPerf = 164;
        heightPerf = 122;
    }
    else
    {
        widthPerf = param->WidthPerf;
        heightPerf = param->HeightPerf;
    }


    // находми правую границу
    if(!GetBlackPixelLine(midX, midY, midX + widthPerf , midY, &pt))
        return false;

    int rightX = pt.x();

    // находми левую границу
    if(!GetBlackPixelLine(midX, midY, midX - widthPerf, midY, &pt))
        return false;

    int leftX = pt.x();
    midX = (rightX + leftX) / 2;

    // находим нижнюю границу
    if(!GetBlackPixelLine( midX, midY, midX, midY + heightPerf, &pt))
        return false;

    int bottomY = pt.y();

    // находим верхнюю границу
    if(!GetBlackPixelLine( midX, bottomY - 20, midX, bottomY - heightPerf - 20, &pt))
        return false;

    int topY = pt.y();
    midY = (bottomY + topY) / 2;

    int y0 = topY + 25;
    int y1 = bottomY - 25;
    QList<int> listTopX;
    QList<int> listBottomX;

    for (int i = y0; i < y1; i += 5)
    {
        if(GetBlackPixelLine( midX, i, leftX - 20, i, &pt))
            listTopX.push_back(pt.x());

        if(GetBlackPixelLine( midX, i, rightX + widthPerf + 20, i, &pt))
            listBottomX.push_back(pt.x());
    }

    if(listBottomX.empty() || listTopX.empty())
        return false;

    rightX = SetStabilValue(listBottomX);
    leftX = SetStabilValue(listTopX);
    midX = (leftX + rightX) / 2;


    int x0 = leftX + 25;
    int x1 = rightX - 25;
    listTopX.clear();
    listBottomX.clear();


    for (int i = x0; i < x1; i += 5)
    {
        if(GetBlackPixelLine( i, midY, i, topY - 20, &pt))
            listTopX.push_back(pt.y());

        if(GetBlackPixelLine( i, midY, i, bottomY + 20, &pt))
            listBottomX.push_back(pt.y());
    }

    if(listBottomX.empty() || listTopX.empty())
        return false;

    bottomY = SetStabilValue(listBottomX);
    topY = SetStabilValue(listTopX);
    midY = (topY + bottomY) / 2;

    if(rc != nullptr)
    {
        rc->setLeft(leftX);
        rc->setRight(rightX);
        rc->setTop(topY);
        rc->setBottom(bottomY);
    }

    return true;

}


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
    Kadr* kadr = nullptr;

    // поисе первой перфорации, определение типа пленки
    GetFirstPerf();

    if(listKadr.isEmpty())
        return;

    Kadr* oldKadr = listKadr.back();
    int endY = img->height() - (param->Height - param->CenterPerfY);

    // добавление следующих кадров
    int x = oldKadr->ptPerf.x();
    for(int y = oldKadr->ptPerf.y() + param->Height; y < endY; y += param->Height)
    {
        kadr = nullptr;
        if(RectPerf(x, y))
        {
            if(type == TypeTape::Super8)
                kadr = new KadrSuper8(this);
            else
                kadr = new Kadr8(this);

            kadr->ptPerf.setX(x);
            kadr->ptPerf.setY(y);
            kadr->Angle = kadr->CalcAngle();

        }
        //Kadr* kadr = FindRectPerf(x, y);
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








