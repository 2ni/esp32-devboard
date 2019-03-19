#ifndef lock_h
#define lock_h

typedef struct Point Point;
typedef struct Dim Dim;
typedef struct Box Box;

struct Point
{
  int x;
  int y;
};

struct Dim
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
