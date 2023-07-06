#ifndef PARAMSKADR_H
#define PARAMSKADR_H

struct ParamKadr
{
public:
    int Height;
    int Width;
    int CenterPerfY;
    int CenterPerfX;
    int WidthPerf;
    int HeightPerf;
    int XCenterFromPerf;
    int YCenterFromPerf;
    int HalfKadrX;
    int HalfKadrY;
};

struct ParamKadr8 : ParamKadr
{
public:
    ParamKadr8()
    {
        HeightPerf = 122;
        WidthPerf = 168;
        CenterPerfY = 291;
        CenterPerfX = 163;
        Height = 352;
        Width = 765;
        XCenterFromPerf = 238;
        YCenterFromPerf = -176; // -115
        HalfKadrX = 556 / 2;
        HalfKadrY = Height / 2;
    }
};

struct ParamKadrSuper8 : ParamKadr
{
public:
    ParamKadrSuper8()
    {
        HeightPerf = 108;
        WidthPerf = 88;
        CenterPerfY = 197;
        CenterPerfX = 89;
        Height = 394;
        Width = 765;
        XCenterFromPerf = 322;
        YCenterFromPerf = 0;
        HalfKadrX = 568 / 2;
        HalfKadrY = Height / 2;
    }
};


#endif // PARAMSKADR_H
