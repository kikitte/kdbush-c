#include <stdlib.h>
#include <math.h>
#include "kdbush.h"

void sort(IndexArrayType *ids, ArrayType *coords, unsigned short nodeSize, int left, int right, int axis);
void selectArray(IndexArrayType *ids, ArrayType *coords, int k, int left, int right, int axis);
void swapItem(IndexArrayType *ids, ArrayType *coords, int i, int j);
void indexSwap(IndexArrayType *arr, int i, int j);
void swap(ArrayType *arr, int i, int j);
double sqDist(double ax, double ay, double bx, double by);

Kdbush *Kdbush_create(unsigned int numItems, unsigned short nodeSize)
{
    KdbushDataHeader *dataHeader = (KdbushDataHeader *)malloc(sizeof(KdbushDataHeader));

    dataHeader->magic = 0xdb;
    dataHeader->versionAndType = 0;
    dataHeader->nodeSize = nodeSize;
    dataHeader->numItens = numItems;

    KdbushData *data = (KdbushData *)malloc(sizeof(KdbushData));

    data->header = dataHeader;
    data->finished = 0;
    data->pos = 0;
    data->ids = (IndexArrayType *)malloc(sizeof(IndexArrayType) * numItems);
    data->coords = (ArrayType *)malloc(sizeof(ArrayType) * numItems * 2);

    Kdbush *kdbush = (Kdbush *)malloc(sizeof(Kdbush));

    kdbush->nodeSize = nodeSize;
    kdbush->numItems = numItems;
    kdbush->data = data;

    return kdbush;
}

void Kdbush_destroy(Kdbush *kdbush)
{
    KdbushData *data = kdbush->data;

    free(data->header);
    free(data->ids);
    free(data->coords);
    free(data);

    free(kdbush);
}

void Kdbush_add(Kdbush *kdbush, ArrayType x, ArrayType y)
{
    KdbushData *data = kdbush->data;

    unsigned int index = data->pos >> 1;
    data->ids[index] = index;
    data->coords[data->pos++] = x;
    data->coords[data->pos++] = y;
}

unsigned int Kdbush_finish(Kdbush *kdbush)
{
    KdbushData *data = kdbush->data;

    unsigned int numAdded = data->pos >> 1;
    if (numAdded != kdbush->numItems)
    {
        return 0;
    }
    else
    {
        // kd-sort both arrays for efficient search
        sort(data->ids, data->coords, kdbush->nodeSize, 0, kdbush->numItems - 1, 0);
        data->finished = 1;
        return 1;
    }
}

KdbushIndexResult *Kdbush_range(Kdbush *kdbush, ArrayType minX, ArrayType minY, ArrayType maxX, ArrayType maxY)
{
    KdbushData *data = kdbush->data;
    if (!data->finished)
    {
        return NULL;
    }

    int stackLength, left, right, axis, m;
    char *axises = (char *)malloc(sizeof(char) * kdbush->numItems);
    IndexArrayType *lefts = (IndexArrayType *)malloc(sizeof(IndexArrayType) * kdbush->numItems);
    IndexArrayType *rights = (IndexArrayType *)malloc(sizeof(IndexArrayType) * kdbush->numItems);
    KdbushIndexResult *result = KdbushIndexResult_create(kdbush->numItems);
    ArrayType x, y;
    ArrayType *coords = data->coords;
    IndexArrayType *ids = data->ids;
    unsigned short nodeSize = kdbush->nodeSize;

    // initizal left, right and axis
    lefts[0] = 0;
    rights[0] = kdbush->numItems - 1;
    axises[0] = 0;
    stackLength = 1;

    // recursively search for items in range in the kd-sorted arrays
    while (stackLength)
    {
        axis = axises[stackLength - 1];
        right = rights[stackLength - 1];
        left = lefts[stackLength - 1];
        --stackLength;

        // if we reached "tree node", search linearly
        if (right - left <= nodeSize)
        {
            for (int i = left; i <= right; i++)
            {
                x = coords[2 * i];
                y = coords[2 * i + 1];
                if (x >= minX && x <= maxX && y >= minY && y <= maxY)
                    result->ids[result->size++] = ids[i];
            }
            continue;
        }

        // otherwise find the middle index
        m = (left + right) >> 1;

        // include the middle item if it's in range
        x = coords[2 * m];
        y = coords[2 * m + 1];
        if (x >= minX && x <= maxX && y >= minY && y <= maxY)
            result->ids[result->size++] = ids[m];

        // queue search in halves that intersect the query
        if (axis == 0 ? minX <= x : minY <= y)
        {
            lefts[stackLength] = left;
            rights[stackLength] = m - 1;
            axises[stackLength] = 1 - axis;
            ++stackLength;
        }
        if (axis == 0 ? maxX >= x : maxY >= y)
        {
            lefts[stackLength] = m + 1;
            rights[stackLength] = right;
            axises[stackLength] = 1 - axis;
            ++stackLength;
        }
    }

    free(axises);
    free(lefts);
    free(rights);

    return result;
}

KdbushIndexResult *Kdbush_within(Kdbush *kdbush, ArrayType qx, ArrayType qy, double r)
{
    KdbushData *data = kdbush->data;
    if (!data->finished)
    {
        return NULL;
    }

    double r2 = r * r;
    int stackLength, left, right, axis, m;
    char *axises = (char *)malloc(sizeof(char) * kdbush->numItems);
    IndexArrayType *lefts = (IndexArrayType *)malloc(sizeof(IndexArrayType) * kdbush->numItems);
    IndexArrayType *rights = (IndexArrayType *)malloc(sizeof(IndexArrayType) * kdbush->numItems);
    KdbushIndexResult *result = KdbushIndexResult_create(kdbush->numItems);
    ArrayType x, y;
    ArrayType *coords = data->coords;
    IndexArrayType *ids = data->ids;
    unsigned short nodeSize = kdbush->nodeSize;

    // initizal left, right and axis
    lefts[0] = 0;
    rights[0] = kdbush->numItems - 1;
    axises[0] = 0;
    stackLength = 1;

    // recursively search for items within radius in the kd-sorted arrays
    while (stackLength)
    {
        axis = axises[stackLength - 1];
        right = rights[stackLength - 1];
        left = lefts[stackLength - 1];
        --stackLength;

        // if we reached "tree node", search linearly
        if (right - left <= nodeSize)
        {
            for (int i = left; i <= right; i++)
            {
                if (sqDist(coords[2 * i], coords[2 * i + 1], qx, qy) <= r2)
                    result->ids[result->size++] = ids[i];
            }
            continue;
        }

        // otherwise find the middle index
        m = (left + right) >> 1;

        // include the middle item if it's in range
        x = coords[2 * m];
        y = coords[2 * m + 1];
        if (sqDist(x, y, qx, qy) <= r2)
            result->ids[result->size++] = ids[m];

        // queue search in halves that intersect the query
        if (axis == 0 ? qx - r <= x : qy - r <= y)
        {
            lefts[stackLength] = left;
            rights[stackLength] = m - 1;
            axises[stackLength] = 1 - axis;
            ++stackLength;
        }
        if (axis == 0 ? qx + r >= x : qy + r >= y)
        {
            lefts[stackLength] = m + 1;
            rights[stackLength] = right;
            axises[stackLength] = 1 - axis;
            ++stackLength;
        }
    }

    free(axises);
    free(lefts);
    free(rights);

    return result;
}

KdbushIndexResult *KdbushIndexResult_create(int maxSize)
{
    KdbushIndexResult *result = (KdbushIndexResult *)malloc(sizeof(KdbushIndexResult));
    result->size = 0;
    result->ids = (IndexArrayType *)malloc(sizeof(IndexArrayType) * maxSize);
    return result;
}

void KdbushIndexResult_destroy(KdbushIndexResult *result)
{
    free(result->ids);
    free(result);
}

void sort(IndexArrayType *ids, ArrayType *coords, unsigned short nodeSize, int left, int right, int axis)
{
    if (right - left <= nodeSize)
        return;

    int m = (left + right) >> 1; // middle index

    // sort ids and coords around the middle index so that the halves lie
    // either left/right or top/bottom correspondingly (taking turns)
    selectArray(ids, coords, m, left, right, axis);

    // recursively kd-sort first half and second half on the opposite axis
    sort(ids, coords, nodeSize, left, m - 1, 1 - axis);
    sort(ids, coords, nodeSize, m + 1, right, 1 - axis);
}

// custom Floyd-Rivest selection algorithm: sort ids and coords so that
// [left..k-1] items are smaller than k-th item (on either x or y axis)
void selectArray(IndexArrayType *ids, ArrayType *coords, int k, int left, int right, int axis)
{
    while (right > left)
    {
        if (right - left > 600)
        {
            int n = right - left + 1;
            int m = k - left + 1;
            double z = log(n);
            double s = 0.5 * exp(2 * z / 3);
            double sd = 0.5 * sqrt(z * s * (n - s) / n) * (m - n / 2 < 0 ? -1 : 1);
            int newLeft = fmax(left, floor(k - m * s / n + sd));
            int newRight = fmin(right, floor(k + (n - m) * s / n + sd));
            selectArray(ids, coords, k, newLeft, newRight, axis);
        }

        double t = coords[2 * k + axis];
        int i = left;
        int j = right;

        swapItem(ids, coords, left, k);
        if (coords[2 * right + axis] > t)
            swapItem(ids, coords, left, right);

        while (i < j)
        {
            swapItem(ids, coords, i, j);
            i++;
            j--;
            while (coords[2 * i + axis] < t)
                i++;
            while (coords[2 * j + axis] > t)
                j--;
        }

        if (coords[2 * left + axis] == t)
            swapItem(ids, coords, left, j);
        else
        {
            j++;
            swapItem(ids, coords, j, right);
        }

        if (j <= k)
            left = j + 1;
        if (k <= j)
            right = j - 1;
    }
}

void swapItem(IndexArrayType *ids, ArrayType *coords, int i, int j)
{
    indexSwap(ids, i, j);
    swap(coords, 2 * i, 2 * j);
    swap(coords, 2 * i + 1, 2 * j + 1);
}

void indexSwap(IndexArrayType *arr, int i, int j)
{
    IndexArrayType tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void swap(ArrayType *arr, int i, int j)
{
    ArrayType tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

double sqDist(double ax, double ay, double bx, double by)
{
    double dx = ax - bx;
    double dy = ay - by;
    return dx * dx + dy * dy;
}
