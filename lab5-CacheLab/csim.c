#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

// #include "cachelab.h"

//opt变量，定义cache大小
int verbose = 0, s, b, E, B, S;

//全局时刻表
int T = 0;

typedef __uint64_t uint64_t;

//定义cache行，在本次实验中不需要存block
typedef struct lineNode{
    int t;  //时刻，也是valid位
    uint64_t tag; //标记
} * groupNode; //定义cache组，即行的数组

groupNode *cache;//定义cache

enum Category
{
    HIT,
    MISS,
    EVICTION
};

const char *categoryString[3] = {"hit ", "miss ", "eviction "};

// 保存统计结果
unsigned int result[3];

void usage(void) { 
    exit(1);
}

//处理命令行选项
FILE *opt(int argc, char **argv){
    FILE *tracefile;
    for (char c; (c = (char)getopt(argc, argv, "hvsEbt")) != EOF;){
        switch(c){
            case 'h':
                //输出帮助文档
                usage();
                break;
            case 'v':
                //输出详细运行过程信息
                verbose = 1;
                break;
            case 's':
                //组索引的位数
                s = atoi(argv[optind]);
                if(s <= 0)
                    usage();
                S = 1 << s;
                break;
            case 'E':
                //每个cache组的行数
                E = atoi(argv[optind]);
                if(E <= 0)
                    usage();
                break;
            case 'b':
                //块偏移的位数
                b = atoi(argv[optind]);
                if(b <= 0)
                    usage();
                B = 1 << b;
                break;
            case 't':
                //输入数据文件的路径
                tracefile = fopen(argv[optind], "r");
                if(tracefile == NULL)
                    usage();
                break;
        }
    }
    return tracefile;
}

//初始化cache
void init_Cache(){
    //cache 有S组，每组有E行
    cache = (groupNode *)malloc(S * sizeof(groupNode));
    for (int i = 0; i < S; i++){
        cache[i] = (struct lineNode *)malloc(E * sizeof(struct lineNode));
        for (int j = 0; j < E; j++){
            cache[i][j].t = 0;
        }
    }
}

//更新cache
void update_Cache(groupNode group, int line_idx, enum Category category, uint64_t tag, char *resultV){
    if(line_idx != -1){
        group[line_idx].t = T;
        group[line_idx].tag = tag;
    }
    result[category]++;
    if(verbose)
        strcat(resultV, categoryString[category]);
}


//查找cache
void search_Cache(uint64_t addr_tag, int group_idx, char *resultV){
    groupNode group = cache[group_idx];
    //min_t_idx用来淘汰最久未访问的行, empty_line_idx用来记录空行
    int min_t_idx = 0, empty_line_idx = -1;
    for (int i = 0; i < E; i++){
        struct lineNode line = group[i];
        if(line.t){ //行非空
            if(line.tag == addr_tag){ // 命中
                update_Cache(group, i, HIT, addr_tag, resultV);
                return;
            }
            if(group[min_t_idx].t > line.t){
                min_t_idx = i;
            }
        }
        else{   //有空行
            empty_line_idx = i;
        }
    }
    // 没有命中
    update_Cache(group, empty_line_idx, MISS, addr_tag, resultV);
    // 需要淘汰
    if(empty_line_idx == -1){
        update_Cache(group, min_t_idx, EVICTION, addr_tag, resultV);
    }
}

void destory(){
    for (int i = 0; i < S; i++){
            free(cache[i]);
    }
    free(cache);
}

int main(int argc, char **argv){
    FILE *tracefile = opt(argc, argv);
    init_Cache();

    //记录文件中每一条指令的类型、存储器地址、访问的字节数
    char operation;
    uint64_t addr;
    int size;
    while(fscanf(tracefile, " %c %lx,%d\n", &operation, &addr, &size) == 3){
        if(operation == 'I')
            continue;
        //取出s位的组索引
        int group_idx = (addr >> b) & ~(~0u << s);
        //取出存储器地址的标志位
        uint64_t addr_tag = addr >> (b + s);
        //全局时刻表自增1
        T++;
        //-v的输出信息
        char resultV[20];
        memset(resultV, 0, sizeof(resultV));
        search_Cache(addr_tag, group_idx, resultV);
        // M操作需要访问两次
        if(operation == 'M'){
            search_Cache(addr_tag, group_idx, resultV);
        }
        if(verbose){
            fprintf(stdout, "%c %lx,%d %s\n", operation, addr, size, resultV);
        }
    }
    printSummary(result[0], result[1], result[2]);
    destory();
    return 0;
}