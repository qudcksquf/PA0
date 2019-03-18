#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 4096
typedef struct CourseEntry {
  char name[MAX_LENGTH]; // 과목의 이름
  struct CourseEntry** prerequisites; // 선수 과목들
  int n_prerequisites; // 선수 과목의 수
  float difficulty; // 과목의 난이도
}ce;

int isToken(char c) {
  return (c == ',' || c == '\n');
}

// arrayList

typedef struct ArrayList {
  int capacity;
  int size;
  ce *arr;
}al;

void create(al *x) {
  x->size = 0;
  x->capacity = 1;
  x->arr = (ce *)malloc(sizeof(ce));
}

ce* ra(al* x, int num) { // 배열 직접 접근
  if(x->size <= num) return NULL;
  else return x->arr + num;
}

void push(al* x, ce t) { // 오직 삽입만 있음 삭제 따윈 없다.
  if(x->size + 1 > x->capacity) {
    ce *tmp = (ce *)malloc(sizeof(ce) * x->capacity);
    for(int i = 0; i < x->size; i++) tmp[i] = x->arr[i];
    free(x->arr);

    x->capacity <<= 1;
    x->arr = (ce *)malloc(sizeof(ce) * x->capacity);
    for(int i = 0; i < x->size; i++) x->arr[i] = tmp[i];
    free(tmp);
  }

  x->arr[x->size++] = t;
}

void clear(al* x) {
  for(int i = 0; i < x->size; i++) free(x->arr[i].prerequisites);
  x->size = 0;
  x->capacity = 0;
  free(x->arr);
}

// end

int main(int argc, char** argv)
{
  FILE *data = fopen("./in/database.csv", "r");

  al li;
  create(&li);

  char ***preReqInfo = (char ***)malloc(sizeof(char **) * 256);

  while(1) {
    char *p = (char *)malloc(sizeof(char) * 256); // buffer size 1 << 8
    fgets(p, 256, data);
    if(feof(data)) {
      free(p);
      break;
    }
    int len = 0;
    int info = 0;
    int cur = 0;
    char *tmp = (char *)malloc(sizeof(char) * 256);
    enum inputs{classInput, sizeInput, preReq};
    for(char *k = p; *k != '\0'; k++) {
      if(isToken(*k) || (*k == ' ' && k != p && (isToken(*(k - 1)) || *(k - 1) == ' '))) {
        tmp[len] = '\0';
        if(len > 0) {
          if(info == classInput) {
            ce makeNode;
            makeNode.difficulty = 5.0;
            strcpy(makeNode.name, tmp);

            push(&li, makeNode);

            info++;
          }
          else if(info == sizeInput) {
            ra(&li, li.size - 1)->n_prerequisites = atoi(tmp);
            ra(&li, li.size - 1)->prerequisites = (ce **)malloc(sizeof(ce *) * (ra(&li, li.size - 1)->n_prerequisites + 1));
            preReqInfo[li.size - 1] = (char **)malloc(sizeof(char *) * (ra(&li, li.size - 1)->n_prerequisites + 1));

            info++;
          }
          else {
            preReqInfo[li.size - 1][cur] = (char *)malloc(sizeof(char) * 256);
            strcpy(preReqInfo[li.size - 1][cur], tmp);
            cur++;
          }
        }
        len = 0;
        continue;
      }
      tmp[len++] = *k;
    }
    free(tmp);
    free(p);
  }

  fclose(data);

  for(int i = 0; i < li.size; i++) {
    for(int j = 0; j < ra(&li, i)->n_prerequisites; j++) {
      for(int k = 0; k < li.size; k++)
        if(strcmp(ra(&li, k)->name, preReqInfo[i][j]) == 0)
          ra(&li, i)->prerequisites[j] = ra(&li, k);
      free(preReqInfo[i][j]);
    }
    free(preReqInfo[i]);
  }
  free(preReqInfo);

  FILE *in = fopen(argv[1], "r");

  while(1) {
    char *p = (char *)malloc(sizeof(char) * 256); // buffer size 1 << 8
    fgets(p, 256, in);
    if(feof(in)) {
      free(p);
      break;
    }

    int len = 0;
    int info = 0;
    int cur = 0;
    char *tmp = (char *)malloc(sizeof(char) * 256);
    enum inputs{classInput, diffcInput};

    float *chng;

    for(char *k = p; *k != '\0'; k++) {
      if(isToken(*k) || (*k == ' ' && k != p && (isToken(*(k - 1)) || *(k - 1) == ' '))) {
        tmp[len] = '\0';
        if(len > 0) {
          if(info == classInput) {
            for(int i = 0; i < li.size; i++) {
              if(strcmp(ra(&li, i)->name, tmp) == 0) {
                chng = &(ra(&li, i)->difficulty);
                break;
              }
            }
            info++;
          }
          else {
            *chng = (float)atof(tmp);
          }
        }
        len = 0;
        continue;
      }
      tmp[len++] = *k;
    }
    free(tmp);
    free(p);
  }

  fclose(in);

  clear(&li);

  return 0;
}
