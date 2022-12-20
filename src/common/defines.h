#include <stdlib.h>
#ifndef SRC_COMMON_DEFINES_H_
#define SRC_COMMON_DEFINES_H_

#define true 1
#define false 0
#define nullptr NULL

typedef int bool;

typedef struct flags {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
  bool b;
  bool t;
} flags;

void flagsInit(flags* f) {
  f->e = false;
  f->i = false;
  f->v = false;
  f->c = false;
  f->l = false;
  f->n = false;
  f->h = false;
  f->s = false;
  f->f = false;
  f->o = false;
  f->b = false;
  f->t = false;
}

void deleteLast(char* s) {
  int len = strlen(s);
  if (len > 1) {
    s[len - 1] = '\0';
  }
}

void deleteGarbage() {
  system("rm -rf .files");
  system("rm -rf .patternFiles");
  system("rm -rf .patterns");
  system("rm -rf .secondFiles");
  system("rm -rf .newPatterns");
}

#endif  // SRC_COMMON_DEFINES_H_
