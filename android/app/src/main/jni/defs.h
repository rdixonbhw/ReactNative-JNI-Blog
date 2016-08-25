typedef struct
{
    double x;
    double y;
} MyVector;

typedef struct
{
    int vector_count;
    MyVector* vectors;
    MyVector position;
} MyStruct;
