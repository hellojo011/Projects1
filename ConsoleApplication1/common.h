typedef int bit;
/*����ü*/
typedef bit chromosome;
typedef struct{
 chromosome *crms;
 double fitness;
} individual;
typedef struct{
 individual *ivd;
 int pop_size;
 int bit_count;
 int data_count;
} population;