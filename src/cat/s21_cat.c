#include <stdio.h>
#include <string.h>

#include "../common/defines.h"

bool checkLong(char* s, flags* f) {
  bool answer = false;
  if (strcmp(s, "number-nonblank") == 0) {
    f->b = true;
    answer = true;
  } else if (strcmp(s, "number") == 0) {
    f->n = true;
    answer = true;
  } else if (strcmp(s, "squeeze-blank") == 0) {
    f->s = true;
    answer = true;
  }
  return answer;
}

bool setFlag(char flag, flags* f) {
  int answer = true;
  switch (flag) {
    case 'b':
      f->b = true;
      f->n = false;
      break;
    case 'e':
      f->e = true;
      f->v = true;
      break;
    case 'E':
      f->e = true;
      break;
    case 'n':
      if (f->b == false) {
        f->n = true;
      }
      break;
    case 's':
      f->s = true;
      break;
    case 't':
      f->t = true;
      f->v = true;
      break;
    case 'T':
      f->t = true;
      break;
    default:
      answer = false;
      break;
  }
  return answer;
}

bool checkFlags(int argc, char** argv, flags* f) {
  int stopIndex = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (checkLong(&argv[i][2], f) == false) {
          printf(
              "cat: illegal option -- -\nusage: cat [-benstuv] [file ...]\n");
          return false;
        }
      } else {
        char* ptr = &argv[i][1];
        while (*ptr != '\0') {
          if (setFlag(*ptr, f) == false) {
            printf(
                "cat: illegal option -- %c\nusage: cat [-benstuv] [file ...]\n",
                *ptr);
            return false;
          }
          ptr++;
        }
      }
    } else {
      stopIndex = i;
      break;
    }
  }
  FILE* files = fopen(".files", "w");
  for (int i = stopIndex; i < argc; i++) {
    fprintf(files, "%s\n", argv[i]);
  }
  fclose(files);
  return true;
}

void cat(flags* f) {
  FILE* files = fopen(".files", "r");
  char path[1024];
  while (fgets(path, 1024, files)) {
    deleteLast(path);
    FILE* file = fopen(path, "r");
    if (file == nullptr) {
      printf("cat: %s: No such file or directory\n", path);
    } else {
      bool lastStrIsEmpty = false;
      char buffer[2048];
      int line = 1;
      while (fgets(buffer, 2048, file)) {
        if (f->s && lastStrIsEmpty && strcmp(buffer, "\n") == 0) {
          continue;

        } else {
          if (f->n) {
            printf("%6d%c", line, 9);
            line++;
          } else if (f->b && strcmp(buffer, "\n") != 0) {
            printf("%6d%c", line, 9);
            line++;
          }
          if (f->e || f->t) {
            for (unsigned long i = 0; i < strlen(buffer); i++) {
              unsigned int c = buffer[i];
              if (f->t && c == '\t') {
                printf("^I");
              } else if (f->v && (c == 127 || c < 32 || c > 127) && c != 10 &&
                         c != 9) {
                if (c == 127) {
                  printf("^?");
                } else if (c < 32) {
                  printf("^%c", c + 64);
                } else {
                  printf("M-");
                  c = c % 128;
                  if (c == 127 || c < 32) {
                    if (c == 127) {
                      printf("^?");
                    } else {
                      printf("^%c", c + 64);
                    }
                  }
                }
              } else if (c != 10) {
                printf("%c", c);
              }
            }
            if (f->e && buffer[strlen(buffer) - 1] == '\n') {
              printf("$");
            }
            printf("\n");
          } else {
            printf("%s", buffer);
          }
        }

        if (strcmp(buffer, "\n") == 0) {
          lastStrIsEmpty = true;
        } else {
          lastStrIsEmpty = false;
        }
      }
    }
    fclose(file);
  }
  fclose(files);
}

int main(int argc, char* argv[]) {
  flags f;
  flagsInit(&f);
  int codeOfParse = checkFlags(argc, argv, &f);
  if (codeOfParse == false) {
    return 0;
  }
  cat(&f);
  deleteGarbage();
  return 0;
}
