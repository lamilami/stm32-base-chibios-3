#ifndef ILI9341_H_
#define ILI9341_H_

typedef union {
  struct {
    int8_t temp[3];
    uint8_t hum[3];
  };
  int8_t val[2][3];
} graph_elem_t;

//#define M5_VALS 36
//#define H1_VALS 24
//#define D1_VALS 14

#define M5_VALS 110
#define H1_VALS 1
#define D1_VALS 1

typedef struct {
  graph_elem_t m5[M5_VALS];
  graph_elem_t h1[24];
  graph_elem_t d1[14];
} graph_t;

void ILI9341_Start(void);

#endif
