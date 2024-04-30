#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <libusb-1.0/libusb.h>

#define CMDLINE_MAX_SIZE 1024
#define MAX_LINE_LENGTH 1024
#define MAX_TOKENS 1024
#define BUFFER_SIZE 1024

#define ANSI_BOLD "\033[1m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

typedef struct {
    char name[256];
    char size[256];
    char used_by[256];
} KernelModule;

struct cpu_info{            //инфморация ядра
    char cpu_product[128];      //продукт
    char cpu_vendor[128];       //производитель
    char cpu_family[128];       //семья процессора
    char cpu_cores[128];        //количество ядер
    char cpu_microcode[128];    //микрокод процуессора
    char cpu_cache[128];        //кеш
    char cpu_bogomips[128];     //скорость исполнения
    char cpu_addres_size[128];  //размеры адресов
};

typedef struct {
    char name[256];
    char rm[256];
    double size;
    char ro[256];
    char mount_point[256];
} DeviceInfo;

void get_proc_info();   //информация о процессах
void cpu_temp();        //температура процессора
void get_system_info(); //информация о системе
void get_cpu_info();    //информация о процессоре
void get_modules_info();//установленные пакеты я ядре
void kernel_info();     //информация о ядре
void get_usb_info();    //информация о подключенныхх устройствах
void get_loop_info();   //информация о дисках
void get_information(); //информация об утилите
void get_network_info();
void print_device_info(const char* device, const char* mountpoint);
void activate(GtkApplication *app, gpointer user_data);    //основное оконное приложение
long uptime();
int compare(const void *a, const void *b);
