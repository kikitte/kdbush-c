#include <stdint.h>

#define VERSION 1 // serialized format version
#define HEADER_SIZE 8

typedef double ArrayType;
typedef unsigned int IndexArrayType;

typedef struct
{
    uint8_t magic;
    uint8_t versionAndType;
    unsigned short nodeSize;
    unsigned int numItens;
} KdbushDataHeader;

typedef struct
{
    KdbushDataHeader *header;
    IndexArrayType *ids;
    ArrayType *coords;
    unsigned int pos;
    char finished;
} KdbushData;

typedef struct
{
    unsigned short nodeSize;
    unsigned int numItems;
    KdbushData *data;
} Kdbush;

typedef struct
{
    unsigned int size;
    IndexArrayType *ids;
} KdbushIndexResult;

Kdbush *Kdbush_create(unsigned int numItems, unsigned short nodeSize);
void Kdbush_destroy(Kdbush *kdbush);
void Kdbush_add(Kdbush *kdbush, ArrayType x, ArrayType y);
KdbushIndexResult *Kdbush_range(Kdbush *kdbush, ArrayType minX, ArrayType minY, ArrayType maxX, ArrayType maxY);
KdbushIndexResult *Kdbush_within(Kdbush *kdbush, ArrayType qx, ArrayType qy, double r);
unsigned int Kdbush_finish(Kdbush *kdbush);
KdbushIndexResult *KdbushIndexResult_create(int maxSize);
void KdbushIndexResult_destroy(KdbushIndexResult *result);
