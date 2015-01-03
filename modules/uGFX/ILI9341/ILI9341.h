#ifndef ILI9341_H_
#define ILI9341_H_

typedef union {
  struct {
    int8_t temp[3];
    uint8_t hum[3];
  };
  int8_t val[2][3];
} graph_elem_t;

//#define S10_VALS 30
//#define M5_VALS 36
//#define H1_VALS 24
//#define D1_VALS 28

#define S10_VALS 30
#define M5_VALS 36
//#define M5_PERIOD 30
#define M5_PERIOD 5
#define H1_VALS 24
//#define H1_PERIOD 12
#define H1_PERIOD 6
#define D1_VALS 28
#define D1_PERIOD 24

typedef union {
  struct {
  graph_elem_t s10[S10_VALS];
  graph_elem_t m5[M5_VALS];
  graph_elem_t h1[H1_VALS];
  graph_elem_t d1[D1_VALS];
  };
  graph_elem_t through[S10_VALS+M5_VALS+H1_VALS+D1_VALS];
} graph_t;

void ILI9341_Start(void);

#endif
