#include <stdio.h>
#include "kdbush.h"

void main()
{
    const int pointNumbers = 100;

    ArrayType points[][2] = {
        {54, 1}, {97, 21}, {65, 35}, {33, 54}, {95, 39}, {54, 3}, {53, 54}, {84, 72}, {33, 34}, {43, 15}, {52, 83}, {81, 23}, {1, 61}, {38, 74}, {11, 91}, {24, 56}, {90, 31}, {25, 57}, {46, 61}, {29, 69}, {49, 60}, {4, 98}, {71, 15}, {60, 25}, {38, 84}, {52, 38}, {94, 51}, {13, 25}, {77, 73}, {88, 87}, {6, 27}, {58, 22}, {53, 28}, {27, 91}, {96, 98}, {93, 14}, {22, 93}, {45, 94}, {18, 28}, {35, 15}, {19, 81}, {20, 81}, {67, 53}, {43, 3}, {47, 66}, {48, 34}, {46, 12}, {32, 38}, {43, 12}, {39, 94}, {88, 62}, {66, 14}, {84, 30}, {72, 81}, {41, 92}, {26, 4}, {6, 76}, {47, 21}, {57, 70}, {71, 82}, {50, 68}, {96, 18}, {40, 31}, {78, 53}, {71, 90}, {32, 14}, {55, 6}, {32, 88}, {62, 32}, {21, 67}, {73, 81}, {44, 64}, {29, 50}, {70, 5}, {6, 22}, {68, 3}, {11, 23}, {20, 42}, {21, 73}, {63, 86}, {9, 40}, {99, 2}, {99, 76}, {56, 77}, {83, 6}, {21, 72}, {78, 30}, {75, 53}, {41, 11}, {95, 20}, {30, 38}, {96, 82}, {65, 48}, {33, 18}, {87, 28}, {10, 10}, {40, 34}, {10, 20}, {47, 29}, {46, 78}};

    IndexArrayType ids[] = {
        97, 74, 95, 30, 77, 38, 76, 27, 80, 55, 72, 90, 88, 48, 43, 46, 65, 39, 62, 93, 9, 96, 47, 8, 3, 12, 15, 14, 21, 41, 36, 40, 69, 56, 85, 78, 17, 71, 44,
        19, 18, 13, 99, 24, 67, 33, 37, 49, 54, 57, 98, 45, 23, 31, 66, 68, 0, 32, 5, 51, 75, 73, 84, 35, 81, 22, 61, 89, 1, 11, 86, 52, 94, 16, 2, 6, 25, 92,
        42, 20, 60, 58, 83, 79, 64, 10, 59, 53, 26, 87, 4, 63, 50, 7, 28, 82, 70, 29, 34, 91};

    ArrayType coords[] = {
        10, 20, 6, 22, 10, 10, 6, 27, 20, 42, 18, 28, 11, 23, 13, 25, 9, 40, 26, 4, 29, 50, 30, 38, 41, 11, 43, 12, 43, 3, 46, 12, 32, 14, 35, 15, 40, 31, 33, 18,
        43, 15, 40, 34, 32, 38, 33, 34, 33, 54, 1, 61, 24, 56, 11, 91, 4, 98, 20, 81, 22, 93, 19, 81, 21, 67, 6, 76, 21, 72, 21, 73, 25, 57, 44, 64, 47, 66, 29,
        69, 46, 61, 38, 74, 46, 78, 38, 84, 32, 88, 27, 91, 45, 94, 39, 94, 41, 92, 47, 21, 47, 29, 48, 34, 60, 25, 58, 22, 55, 6, 62, 32, 54, 1, 53, 28, 54, 3,
        66, 14, 68, 3, 70, 5, 83, 6, 93, 14, 99, 2, 71, 15, 96, 18, 95, 20, 97, 21, 81, 23, 78, 30, 84, 30, 87, 28, 90, 31, 65, 35, 53, 54, 52, 38, 65, 48, 67,
        53, 49, 60, 50, 68, 57, 70, 56, 77, 63, 86, 71, 90, 52, 83, 71, 82, 72, 81, 94, 51, 75, 53, 95, 39, 78, 53, 88, 62, 84, 72, 77, 73, 99, 76, 73, 81, 88,
        87, 96, 98, 96, 82};

    Kdbush *kdbush = Kdbush_create(pointNumbers, 10);
    for (int i = 0; i < pointNumbers; ++i)
    {
        Kdbush_add(kdbush, points[i][0], points[i][1]);
    }
    Kdbush_finish(kdbush);

    // coords are kd-sorted
    for (int i = 0; i < pointNumbers; ++i)
    {
        int j = 2 * i;
        if (coords[j] != kdbush->data->coords[j] && coords[j + 1] != kdbush->data->coords[j + 1])
        {
            printf("coords are not kd-sorted: %d", i);
            return;
        }
    }
    puts("\n");

    // ids are kd-sorted
    for (int i = 0; i < pointNumbers; ++i)
    {
        if (ids[i] != kdbush->data->ids[i])
        {
            printf("ids are not kd-sorted: %d", i);
            return;
        }
    }

    // range search
    IndexArrayType rangePointsIndex[] = {60, 20, 45, 3, 17, 71, 44, 19, 18, 15, 69, 90, 62, 96, 47, 8, 77, 72};
    KdbushIndexResult *rangeSearchResult = Kdbush_range(kdbush, 20, 30, 50, 70);
    if (rangeSearchResult->size != 18)
    {
        printf("range search returns incorrect size: %d", rangeSearchResult->size);
    }
    else
    {
        for (int i = 0; i < rangeSearchResult->size; ++i)
        {
            if (rangePointsIndex[i] != rangeSearchResult->ids[i])
            {
                printf("range search returns incorrect index: i = %d, index = %d", i, rangeSearchResult->ids[i]);
            }
        }
    }

    KdbushIndexResult_destroy(rangeSearchResult);

    // radius search
    double qx = 50, qy = 50, r = 20;
    IndexArrayType radiusPointsIndex[] = {60, 6, 25, 92, 42, 20, 45, 3, 71, 44, 18, 96};
    KdbushIndexResult *radiusSearchResult = Kdbush_within(kdbush, qx, qy, r);
    if (radiusSearchResult->size != 12)
    {
        printf("radius search returns incorrect size: %d", radiusSearchResult->size);
    }
    else
    {
        for (int i = 0; i < radiusSearchResult->size; ++i)
        {
            if (radiusPointsIndex[i] != radiusSearchResult->ids[i])
            {
                printf("radius search returns incorrect index: i = %d, index = %d", i, radiusSearchResult->ids[i]);
            }
        }
    }
    KdbushIndexResult_destroy(radiusSearchResult);

    Kdbush_destroy(kdbush);
}