#include <stdlib.h>
#include <time.h>
#include <stdio.h>

typedef struct {
    int *ds;
    char **edges;
    int size;
} Maze_info;

// Maze_info의 포인터를 받아서 int array ds와 char double dimension array edges를 initialize함.
void init_maze_info(Maze_info *maze)
{
    int i, j;
    maze->ds = malloc(sizeof(int)*(maze->size*maze->size + 1));
    for(i = 1; i <= maze->size * maze->size; ++i) {
        maze->ds[i] = 0;
    }

    maze->edges = malloc(sizeof(char*)*(2*maze->size - 1));
    for(i = 0; i < 2*maze->size - 1; ++i) {
        maze->edges[i] = malloc(sizeof(char)*maze->size);
        for(j = 0; j < maze->size; ++j) {
            maze->edges[i][j] = (i%2 == 0) ? '|' : '-';
        }
    }
}

// disjoint sets, ds에서 a를 find한다. a가 속한 set의 root를 return.s
int find(int *ds, int a)
{
    if(ds[a] > 0) {
        return ds[a] = find(ds, ds[a]);
    }
    return a;
}

// disjoint sets, ds에서 small_num과 large_num의 root를 find한 후, 둘을 union할 수 있다면, union한다.
void union_ds(int *ds, int small_num, int large_num)
{
    small_num = find(ds, small_num);
    large_num = find(ds, large_num);
    if(ds[small_num] > ds[large_num]) {
        ds[small_num] = large_num;
    } else {
        if(ds[small_num] == ds[large_num]) {
            --ds[small_num];
        }
        ds[large_num] = small_num;
    }
}

// edge의 x좌표와 y좌표를 받아서 그 edge에 adjacent한 칸을 union하고 edge를 지운다.
void pick(Maze_info *maze, int x, int y)
{
    int small_num, large_num; //small, big는 해당하는 숫자 두개.
    int i, j; //index for for
    int small_num_root, large_num_root;

    //위아래일 때, a가 위, b가 아래
    if(y % 2) {
        small_num = (y/2) * maze->size + x + 1;
        large_num = (y/2 + 1) * maze->size + x + 1;
    } else { //옆일때, a가 왼, b가 오
        x = x % (maze->size - 1);
        small_num = (y/2) * maze->size + x + 1;
        large_num = (y/2) * maze->size + x + 2;
    }

    small_num_root = find(maze->ds, small_num);
    large_num_root = find(maze->ds, large_num);

    if(small_num_root == large_num_root) return;

    union_ds(maze->ds, small_num, large_num);

    maze->edges[y][x] = ' ';
}

// Maze_info 변수의 내용을 free해준다.
void delete_maze_info(Maze_info *maze)
{
    int i;
    for(i = 0; i < 2*maze->size - 1; ++i) {
        free(maze->edges[i]);
    }
    free(maze->edges);
    free(maze->ds);
}

// disjoint sets, ds에 복수의 set이 존재하면 1을, 1개의 set으로 합쳐져 있으면 0을 return
short int exist_sets(Maze_info *maze)
{
    int i;
    short int count = 0;
    for(i = 1; i <= maze->size * maze->size; ++i) {
        if(maze->ds[i] <= 0) {
            ++count;
        }
        if(count > 1) {
            return 1;
        }
    }
    return 0;
}

// Maze_info 변수의 정보를 보고, maze를 print 해준다.
void print_maze(FILE *output, Maze_info *maze)
{
    int i, j;
    for(j = 0; j <= 2 * maze->size; ++j) {
        fprintf(output, (j%2) ? "-" : "+");
    }
    fprintf(output, "\n");
    for(i = 1; i < 2 * maze->size; ++i, fprintf(output, "\n")) {
        if(i % 2) {
            if(i/2 == 0) fprintf(output, " ");
            else fprintf(output, "|");
            for(j = 1; j < 2 * maze->size; ++j) {
                if(j % 2) {
                    fprintf(output, " ");
                    continue;
                }
                fprintf(output, "%c", maze->edges[i - 1][j/2 - 1]);
            }
            if(i/2 == maze->size - 1) fprintf(output, " ");
            else fprintf(output, "|");
        } else {
            for(j = 0; j <= 2 * maze->size; ++j) {
                if(j % 2 == 0) {
                    fprintf(output, "+");
                    continue;
                }
                fprintf(output, "%c", maze->edges[i - 1][j/2]);
            }
        }
    }
    for(j = 0; j <= 2 * maze->size; ++j) {
        fprintf(output, (j%2) ? "-" : "+");
    }
    fprintf(output, "\n");
}

int main()
{
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    Maze_info maze_info;
    int i;

    srand((unsigned int)time(NULL));
    fscanf(input, "%d", &maze_info.size);

    init_maze_info(&maze_info);

    while(exist_sets(&maze_info)) {
        pick(&maze_info, rand() % maze_info.size, rand() % (2 * maze_info.size - 1));
    }

    print_maze(output, &maze_info);
    delete_maze_info(&maze_info);

    fclose(input);
    fclose(output);
    return 0;
}
