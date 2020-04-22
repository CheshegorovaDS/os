#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
//2. Создайте программу для копирования со стандартного устройства ввода на
//стандартное устройства вывода. Размер буфера, используемого в системных
//вызовах read и write,передавать в качестве параметра через командную строку.
//Далее исследуйте зависимость операций чтения/записи с различными размерами
//буфера для файлов большого размера (несколько сотен мегабайт).

int main(int argc, char** argv) {
    int buf_size = atoi(argv[1]);

    char buffer[buf_size];
     mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

     ssize_t read_bytes;
     ssize_t written_bytes;

     if (argc < 2)
     {
         fprintf (stderr, "Too few arguments\n");
         exit (1);
     }

     while ((read_bytes = read (0, buffer, buf_size)) > 0)
     {
         written_bytes = write (1, buffer, read_bytes);
         if (written_bytes != read_bytes)
         {
             fprintf (stderr, "Cannot write\n");
             exit (1);
         }
     }

     if (read_bytes < 0)
     {
         fprintf (stderr, " Cannot read file\n");
         exit (1);
     }
     exit (0);
 }
