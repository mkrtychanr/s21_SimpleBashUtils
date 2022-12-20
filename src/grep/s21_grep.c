#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/defines.h"

int setFlag(char flag, flags* f) {
  int answer = 1;
  switch (flag) {
    case 'e':
      f->e = true;
      break;
    case 'i':
      f->i = true;
      break;
    case 'v':
      f->v = true;
      break;
    case 'c':
      f->c = true;
      break;
    case 'l':
      f->l = true;
      break;
    case 'n':
      f->n = true;
      break;
    case 'h':
      f->h = true;
      break;
    case 's':
      f->s = true;
      break;
    case 'f':
      f->f = true;
      break;
    case 'o':
      f->o = true;
      break;
    default:
      answer = 0;
      break;
  }
  return answer;
}

void writeToFile(int argc, char** argv, int i, int j, FILE* file) {
  if (j < (int)strlen(argv[i]) - 1) {
    for (j++; j < (int)strlen(argv[i]); j++) {
      fprintf(file, "%c", argv[i][j]);
    }
    fprintf(file, "\n");
  } else {
    if (i < argc - 1) {
      fprintf(file, "%s\n", argv[i + 1]);
      i++;
    }
  }
}

bool checkFlags(int argc, char** argv, flags* f) {
  FILE* patternFiles = fopen(".patternFiles", "a");
  FILE* patterns = fopen(".patterns", "a");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; j < (int)strlen(argv[i]); j++) {
        if (argv[i][j] == 'e' || argv[i][j] == 'f') {
          if (argv[i][j] == 'e') {
            writeToFile(argc, argv, i, j, patterns);
            f->e = true;
          } else {
            writeToFile(argc, argv, i, j, patternFiles);
            f->f = true;
          }
          break;
        } else if (!setFlag(argv[i][j], f)) {
          printf("grep: invalid option -- %c\n", argv[i][j]);
          printf(
              "usage: grep [-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] "
              "[-C[num]]\n");
          printf(
              "        [-e pattern] [-f file] [--binary-files=value] "
              "[--color=when]\n");
          printf(
              "        [--context[=num]] [--directories=action] [--label] "
              "[--line-buffered]\n");
          printf("        [--null] [pattern] [file ...]\n");
          fclose(patterns);
          fclose(patternFiles);
          return 0;
        }
      }
    }
  }
  fclose(patterns);
  fclose(patternFiles);
  return 1;
}

void createTempFile() {
  FILE* patternFiles = fopen(".patternFiles", "w");
  FILE* patterns = fopen(".patterns", "w");
  FILE* words = fopen(".files", "w");
  fclose(patternFiles);
  fclose(patterns);
  fclose(words);
}

bool fillPatternsFromFiles() {
  FILE* patternFiles = fopen(".patternFiles", "r");
  FILE* patterns = fopen(".patterns", "a");
  char path[1024];
  while (fgets(path, 1024, patternFiles)) {
    deleteLast(path);
    FILE* file = fopen(path, "r");
    if (file == nullptr) {
      printf("grep: %s: No such file or directory\n", path);
      fclose(patternFiles);
      fclose(patterns);
      return 0;
    }
    char pattern[1024];
    while (fgets(pattern, 1024, file)) {
      fprintf(patterns, "%s", pattern);
    }
    fclose(file);
  }
  fclose(patternFiles);
  fclose(patterns);
  return 1;
}

void fillFiles(int argc, char** argv) {
  FILE* files = fopen(".files", "a");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (argv[i - 1][0] == '-') {
        int len = (int)strlen(argv[i - 1]);
        if (argv[i - 1][len - 1] != 'f' && argv[i - 1][len - 1] != 'e') {
          fprintf(files, "%s\n", argv[i]);
        }
      } else {
        fprintf(files, "%s\n", argv[i]);
      }
    }
  }
  fclose(files);
}

void oldGrep(flags* f, int quantityOfFiles) {
  FILE* files = fopen(".files", "r");
  char path[1024];
  while (fgets(path, 1024, files)) {
    deleteLast(path);
    FILE* file = fopen(path, "r");
    if (file == nullptr) {
      if (f->s == false) {
        printf("grep: %s: No such file or directory\n", path);
      }
      continue;
    }
    char line[1024];
    int lineCounter = 0;
    int matchCounter = 0;
    FILE* patterns = fopen(".patterns", "r");
    char pattern[1024];
    regex_t regex;
    // returnValue = regcomp(&regex, pattern, 0);
    int flagI = 0;
    if (f->i == true) {
      flagI = REG_ICASE;
    }
    while (fgets(pattern, 1024, patterns)) {
      deleteLast(pattern);
      regcomp(&regex, pattern, flagI);
    }
    while (fgets(line, 1024, file)) {
      lineCounter++;
      FILE* patterns = fopen(".patterns", "r");
      char pattern[1024];
      while (fgets(pattern, 1024, patterns)) {
        deleteLast(pattern);
        regex_t regex;
        int returnValue = 0;
        regmatch_t find;
        returnValue = regcomp(&regex, pattern, 0);
        returnValue = regexec(&regex, line, 1, &find, 0);
        if (f->v == true && returnValue == REG_NOMATCH) {
          if (f->l == false && f->c == false) {
            if (f->h == false && quantityOfFiles != 1) {
              printf("%s:", path);
            }
            if (f->n == true) {
              printf("%d:", lineCounter);
            }
            printf("%s", line);
            regfree(&regex);
            break;
          }
          matchCounter++;
        } else if (f->v == false && returnValue == 0) {
          if (f->l == false && f->c == false) {
            if (f->h == false && quantityOfFiles != 1) {
              printf("%s:", path);
            }
            if (f->n == true) {
              printf("%d:", lineCounter);
            }
            printf("%s", line);
            regfree(&regex);
            break;
          }
          matchCounter++;
        }
        regfree(&regex);
      }
      fclose(patterns);
    }
    bool flag = false;
    if (matchCounter == 0) {
      flag = true;
    }
    if (f->c == true) {
      if (f->h == false && quantityOfFiles != 1) {
        printf("%s:", path);
      }
      if (f->l == true && flag == false) {
        matchCounter = 1;
      }
      printf("%d\n", matchCounter);
    }
    if (f->l == true && flag == false) {
      printf("%s\n", path);
    }
    fclose(file);
  }
  fclose(files);
}

void printLine(flags* f, int quantityOfFiles, char* path, char* line,
               int lineCounter) {
  if (f->l == false && f->c == false) {
    if (f->h == false && quantityOfFiles != 1) {
      printf("%s:", path);
    }
    if (f->n == true) {
      printf("%d:", lineCounter);
    }
    printf("%s", line);
  }
}

void grep(flags* f, int quantityOfFiles, int quantityOfPatterns) {
  FILE* patterns = fopen(".patterns", "r");
  FILE* files = fopen(".files", "r");
  char pattern[1024];
  char path[1024];
  regex_t* regex = malloc(quantityOfPatterns * sizeof(regex_t));
  int flag = 0;
  if (f->i == true) {
    flag = REG_ICASE;
  }
  int counter = 0;
  while (fgets(pattern, 1024, patterns)) {
    deleteLast(pattern);
    regcomp(&regex[counter++], pattern, flag);
  }
  fclose(patterns);
  while (fgets(path, 1024, files)) {
    deleteLast(path);
    FILE* file = fopen(path, "r");
    if (file == nullptr) {
      if (f->s == false) {
        printf("grep: %s: No such file or directory\n", path);
      }
      continue;
    }
    char line[1024];
    int lineCounter = 0;
    int matchCounter = 0;
    bool fileFlag = false;
    while (fgets(line, 1024, file)) {
      lineCounter++;
      regmatch_t find;
      for (int i = 0; i < quantityOfPatterns; i++) {
        int returnValue = regexec(&regex[i], line, 0, &find, 0);
        if (f->v == true && returnValue == REG_NOMATCH) {
          printLine(f, quantityOfFiles, path, line, lineCounter);
          matchCounter++;
          if (f->l == true) {
            fileFlag = true;
          }
          break;
        }
        if (f->v == false && returnValue == 0) {
          printLine(f, quantityOfFiles, path, line, lineCounter);
          matchCounter++;
          if (f->l == true) {
            fileFlag = true;
          }
          break;
        }
      }
      if (fileFlag) {
        break;
      }
    }
    if (f->c == true) {
      if (f->h == false && quantityOfFiles != 1) {
        printf("%s:", path);
      }
      printf("%d\n", matchCounter);
    }
    if (f->l == true && matchCounter != 0) {
      printf("%s\n", path);
    }
    fclose(file);
  }
  for (int i = 0; i < quantityOfPatterns; i++) {
    regfree(&regex[i]);
  }
  free(regex);
  fclose(files);
}

int countFiles() {
  FILE* files = fopen(".files", "r");
  int answer = 0;
  char file[1024];
  while (fgets(file, 1024, files)) {
    answer++;
  }
  return answer;
}

int countPatterns() {
  FILE* patterns = fopen(".patterns", "r");
  int answer = 0;
  char pattern[1024];
  while (fgets(pattern, 1024, patterns)) {
    answer++;
  }
  return answer;
}

void magic() {
  FILE* files = fopen(".files", "r");
  FILE* patterns = fopen(".patterns", "a");
  FILE* secondFiles = fopen(".secondFiles", "w");
  char file[1024];
  char* lastCall = fgets(file, 1024, files);
  fprintf(patterns, "%s", file);
  lastCall = fgets(file, 1024, files);
  while (lastCall) {
    fprintf(secondFiles, "%s", file);
    lastCall = fgets(file, 1024, files);
  }
  fclose(secondFiles);
  fclose(patterns);
  fclose(files);
  files = fopen(".files", "w");
  secondFiles = fopen(".secondFiles", "r");
  while (fgets(file, 1024, secondFiles)) {
    fprintf(files, "%s", file);
  }
  fclose(files);
  fclose(secondFiles);
}

int main(int argc, char* argv[]) {
  flags f;
  flagsInit(&f);
  createTempFile();
  if (!checkFlags(argc, argv, &f)) {
    deleteGarbage();
    return 0;
  }
  fillFiles(argc, argv);
  if (f.f == true) {
    if (fillPatternsFromFiles() == false) {
      deleteGarbage();
      return 0;
    }
  }
  if (f.f == false && f.e == false) {
    magic();
  }
  int files = countFiles();
  int patterns = countPatterns();
  grep(&f, files, patterns);
  deleteGarbage();
  return 0;
}
