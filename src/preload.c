/*
    YAGF - cuneiform and tesseract OCR graphical front-ends
    Copyright (C) 2009-2010 Andrei Borovsky <anb@symmetrica.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>

//static int fd = 0;
static FILE * f;

/*int open(const char *pathname, int flags)
{
        printf("called %s\n", pathname);
}*/
FILE* fopen(const char* path, const char* mode) {
    printf("called %s\n", path);
FILE* (*real_fopen)(const char*, const char*) =
dlsym(RTLD_NEXT, "fopen");
    if (strstr(path, "input.png")== NULL)
        return real_fopen(path, mode);
if (strstr(mode, "r"))
    return 0;
//if (fd <= 0)
//fd = open("/var/tmp/yagf.fifo", O_WRONLY);
f = real_fopen(path, mode);
return f;
}

/*size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                     FILE *stream)
{
    FILE* (*real_fwrite)(const void *ptr, size_t size, size_t nmemb,
                     FILE *stream) =
dlsym(RTLD_NEXT, "fwrite");
    if (stream == f)
        write(fd, ptr, size*nmemb);
   return real_fwrite(ptr, size, nmemb, stream);

}*/

int fclose(FILE *fp) {
    int (* real_fclose)(FILE *fp) = dlsym(RTLD_NEXT, "fclose");
    if (f == fp) {
   //     write(fd, endmark, 6);
        int ppid = getppid();
        kill(ppid, SIGUSR2);
    }
    return real_fclose(fp);
}
