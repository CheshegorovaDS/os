#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//3.1
//Реализовать функцию обхода дерева каталогов снизу
//вверх (аналог функции getcwd) с подчетом
//полного числа символических ссылок
//на исходный каталог.

int countsym()
{
    int count = 0;
    DIR* cur_dir;
    int distance = 0;

    struct stat cur;
    stat(".", &cur);

    struct stat rootstat;
    stat("/",&rootstat);

    struct dirent* elem;
    struct stat elem_info;
    struct stat cur_dir_info;

    while(1) {

        cur_dir = opendir(".");
        if(cur_dir == NULL){
            fprintf(stderr,"error. can't open dir");
            exit(1);
        }
        stat(".",&cur_dir_info);
        if(distance % 2 == 0) {

            while ((elem = readdir(cur_dir)) != NULL) {
                lstat(elem->d_name, &elem_info);
                if (S_ISLNK(elem_info.st_mode) != 0) {
                    stat(elem->d_name, &elem_info);
                    if (elem_info.st_dev == cur.st_dev && elem_info.st_ino == cur.st_ino) {
                        count++;
                    }
                }
            }
        }

        if(cur_dir_info.st_dev == rootstat.st_dev && cur_dir_info.st_ino == rootstat.st_ino ){
            return count;
        }

        chdir("..");
        distance++;
    }
}

int main() {
    printf(" Количество символьных ссылок: %d",countsym());
    exit(0);
}
