#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

#define BUF_LEN 256
#define CLIENT_COUNT 10
//  Реализовать клиент‐серверное приложение,
//  в котором клиенты и	 сервер	 реализованы в
//   виде потоков. Сервер‐писатель считывает строку
//   со стандартного ввода и передает ее через буфер
//  клиентам. Клиенты-читатели считывают строку	 из
//  буфера и записывают ее в файл (один и тот же для
//  всех клиентов).
//  Сервер заканчивает свою работу при	получении
//  со стандартного ввода строки  «quit».
//  Клиенты заканчивают работу при чтении
//  из буфера данной строки (в файл эту строку не пишут).
//   Для организации взаимного исключения к буферу
//  и файлам использовать блокировки чтения‐записи.

char buf[BUF_LEN];
int arr_client[CLIENT_COUNT];
FILE* file;
pthread_rwlock_t rwlock_buf;
pthread_rwlock_t rwlock_file;
pthread_rwlock_t rwlock;

void* client(void *args) {
    int* idClient = args;
    for(;;){
        pthread_rwlock_rdlock(&rwlock_buf);
        pthread_rwlock_wrlock(&rwlock);

        if(arr_client[*idClient] && strlen(buf) != 0) {
            printf("client id =  %d -> %s\n", *idClient, buf);
            if (strcmp(buf, "quit\n") == 0) {
                pthread_rwlock_unlock(&rwlock_buf);
                pthread_rwlock_unlock(&rwlock);
                printf("Client close id = %d\n",*idClient);
                return 0;
            }
                pthread_rwlock_wrlock(&rwlock_file);
                if(file != NULL){
                    fputs(buf,file);
                }
                pthread_rwlock_unlock(&rwlock_file);
                arr_client[*idClient] = 0;
        }
        pthread_rwlock_unlock(&rwlock_buf);
        pthread_rwlock_unlock(&rwlock);
        sched_yield();
    }
}

void* server(void *args) {
    char* in;
    for (;;) {
        pthread_rwlock_wrlock(&rwlock_buf);
        pthread_rwlock_wrlock(&rwlock);

        int count_reader = 0;
        for(int i = CLIENT_COUNT - 1; i >= 0; i--){
            if(!arr_client[i]){
                count_reader++;
            }
        }
        if(count_reader == CLIENT_COUNT && (in = fgets(buf, BUF_LEN, stdin)) != NULL){
            printf("server -> %s\n", buf);

            for(int i = 0; i<CLIENT_COUNT; i++){
                arr_client[i] = 1;
            }
            if (strcmp(in, "quit\n") == 0) {
                break;
            }
        }
        pthread_rwlock_unlock(&rwlock_buf);
        pthread_rwlock_unlock(&rwlock);
        sched_yield();
    }
    pthread_rwlock_unlock(&rwlock_buf);
    pthread_rwlock_unlock(&rwlock);
    printf("Server close\n");
    return 0;
}


int main()
{
    int status_addr;
    int status;
    int args[CLIENT_COUNT];
    pthread_t server_thread;
    pthread_t client_threads[CLIENT_COUNT];

    if(( file = fopen("a.txt","w+"))  == NULL){
        fprintf(stderr,"Cannot open file.\n");
        exit(1);
    }

    if(pthread_rwlock_init(&rwlock_buf, 0) != 0){
        fprintf(stderr,"Cannot init rwlock_buf.\n");
        return 1;
    }

    if(pthread_rwlock_init(&rwlock_file, 0) != 0){
        fprintf(stderr,"Cannot init rwlock_file.\n");
        return 1;
    }

    if(pthread_rwlock_init(&rwlock, 0) != 0){
        fprintf(stderr,"Cannot init rwlock.\n");
        return 1;
    }

    for(int i = 0; i< CLIENT_COUNT;i++) {
        args[i] = i;
        if (pthread_create(&client_threads[i], NULL, client, (void *) &args[i]) != 0) {
            fprintf(stderr, "Can't create thread-client.\n");
            return 1;
        }
    }

    if(pthread_create(&server_thread,NULL,server,NULL) != 0){
        fprintf(stderr,"Can't create thread-server.\n");
        return 1;
    }

    for(int i = 0; i< CLIENT_COUNT; i++) {
        status = pthread_join(client_threads[i], (void **) &status_addr);
        if (status != 0) {
            printf("main error: can't join thread, status = %d\n", status);
        }
    }

    status = pthread_join(server_thread, (void**)&status_addr);
    if (status != 0) {
        printf("main error: can't join thread, status = %d\n", status);
    }

    if(pthread_rwlock_destroy(&rwlock_buf) != 0){
        fprintf(stderr,"Cannot destroy rwlock_buf.\n");
        return 1;
    }

    if(pthread_rwlock_destroy(&rwlock_file) != 0){
        fprintf(stderr,"Cannot destroy rwlock_file.\n");
        return 1;
    }

    if(pthread_rwlock_destroy(&rwlock) != 0){
        fprintf(stderr,"Cannot destroy rwlock.\n");
        return 1;
    }

    if(fclose(file) != 0 ){
        fprintf(stderr,"Cannot close file.\n");
        return 1;
    }

   exit(0);
}