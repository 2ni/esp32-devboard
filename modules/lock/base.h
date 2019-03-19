#ifndef base_h
#define base_h

typedef struct Point Point;
typedef struct Dimension Dimension;
typedef struct Box Box;

struct Point
{
  int x;
  int y;
};

struct Dimension
{
  int width;
  int height;
};

struct Box
{
  int x;
  int y;
  int width;
  int height;
};

#endif
