#if !defined(test_basic_h)
#define test_basic_h 1

typedef enum {PROT_CHAR, PROT_UCHAR,
              PROT_SHORT, PROT_USHORT,
              PROT_INT, PROT_UINT,
              PROT_LONG, PROT_ULONG,
              PROT_FLOAT, PROT_DOUBLE,
              PROT_STRING, PROT_ALL, PROT_EXIT} Protocol;

#endif /* test_basic_h */