#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 4096
typedef struct CourseEntry {
  int pre_order; // 위상정렬을 위한 pre order
  int post_order; // 위상정렬을 위한 post order
  char name[MAX_LENGTH]; // 과목의 이름
  struct CourseEntry** postrequisites; // 선수 과목들
  int n_postrequisites; // 이 과목을 들어야 다음 들을 수 있는 과목
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
  for(int i = 0; i < x->size; i++) free(x->arr[i].postrequisites);
  x->size = 0;
  x->capacity = 0;
  free(x->arr);
}

// end

void dfs(ce *u, int *cnt) {
  if(u->pre_order) return;
  u->pre_order = ++(*cnt);
  for(int i = 0; i < u->n_postrequisites; i++) dfs(u->postrequisites[i], cnt);
  u->post_order = ++(*cnt);
}

int main(int argc, char** argv)
{
  FILE *data = fopen("./in/database.csv", "r");

  al li;
  create(&li);

  int *cntReq = (int *)malloc(sizeof(int) * 256);
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
            makeNode.n_postrequisites = 0;
            makeNode.difficulty = 5.0;
            strcpy(makeNode.name, tmp);

            push(&li, makeNode);

            info++;
          }
          else if(info == sizeInput) {
            cntReq[li.size - 1] = atoi(tmp);
            preReqInfo[li.size - 1] = (char **)malloc(sizeof(char *) * (cntReq[li.size - 1] + 1));

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
            for(int i = 0; i < li.size; i++) { // sequential search O(N)
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

  for(int i = 0; i < li.size; i++) {
    for(int j = i + 1; j < li.size; j++) {
      ce *x = ra(&li, i);
      ce *y = ra(&li, j);
      if(x->difficulty > y->difficulty){
        ce tmp = *x;
        *x = *y;
        *y = tmp;

        char **ctmp = preReqInfo[i];
        preReqInfo[i] = preReqInfo[j];
        preReqInfo[j] = ctmp;

        int crtmp = cntReq[i];
        cntReq[i] = cntReq[j];
        cntReq[j] = crtmp;
      }
      else if(x->difficulty == y->difficulty && strcmp(x->name, y->name) < 0) {
        ce tmp = *x;
        *x = *y;
        *y = tmp;

        char **ctmp = preReqInfo[i];
        preReqInfo[i] = preReqInfo[j];
        preReqInfo[j] = ctmp;

        int crtmp = cntReq[i];
        cntReq[i] = cntReq[j];
        cntReq[j] = crtmp;
      }
    }
  }

  for(int i = 0; i < li.size; i++) {
    for(int j = 0; j < cntReq[i]; j++) {
      for(int k = 0; k < li.size; k++)
        if(strcmp(ra(&li, k)->name, preReqInfo[i][j]) == 0) // sequential search O(N)
          ra(&li, k)->n_postrequisites++;
    }
  }

  for(int i = 0; i < li.size; i++) ra(&li, i)->postrequisites = (ce **)malloc(sizeof(ce *) * (ra(&li, i)->n_postrequisites));
  for(int i = 0; i < li.size; i++) {
    int cur = 0;
    for(int j = 0; j < cntReq[i]; j++) {
      for(int k = 0; k < li.size; k++)
        if(strcmp(ra(&li, k)->name, preReqInfo[i][j]) == 0) // sequential search O(N)
          ra(&li, k)->postrequisites[cur] = ra(&li, i);
      free(preReqInfo[i][j]);
    }
    free(preReqInfo[i]);
  }
  free(preReqInfo);
  free(cntReq);

  for(int k = 0; k < li.size; k++) { // O(V^2) Selection Sort
    ce *cur = ra(&li, k);
    for(int i = 0; i < cur->n_postrequisites; i++) {
      for(int j = i + 1; j < cur->n_postrequisites; j++) {
        ce *x = cur->postrequisites[i];
        ce *y = cur->postrequisites[j];
        if(x->difficulty > y->difficulty){
          ce tmp = *x;
          *x = *y;
          *y = tmp;
        }
        else if(x->difficulty == y->difficulty && strcmp(x->name, y->name) < 0) {
          ce tmp = *x;
          *x = *y;
          *y = tmp;
        }
      }
    }
  }

  int cnt = 0;
  for(int i = 0; i < li.size; i++) dfs(ra(&li, i), &cnt); // O(V + E) Time Complexity

  for(int i = 0; i < li.size; i++) { // O(V^2) Selection Sort
    for(int j = i + 1; j < li.size; j++) {
        ce *x = ra(&li, i);
        ce *y = ra(&li, j);
        if(x->post_order < y->post_order){
          ce tmp = *x;
          *x = *y;
          *y = tmp;
        }
    }
  }

  for(int i = 0; i < li.size; i++) {
    printf("%s\n", ra(&li, i)->name);
  }

  clear(&li);

  return 0;
}
