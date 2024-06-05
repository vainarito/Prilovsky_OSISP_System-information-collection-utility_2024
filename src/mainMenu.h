#ifndef MAINMENU_H
#define MAINMENU_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <libusb-1.0/libusb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/inet_diag.h>

#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"

#define BUFFER_SIZE 1024
#define DEVICE_NAME_SIZE 256
#define DEVICE_RM_SIZE 4
#define DEVICE_MOUNT_POINT_SIZE 256
#define MODULE_NAME_SIZE 256
#define MODULE_SIZE_SIZE 256
#define MODULE_USED_BY_SIZE 256
#define TIME_SIZE 32
#define HDD_SIZE 256

struct cpu_info {
    char cpu_product[BUFFER_SIZE]; // Модель процессора.
    char cpu_vendor[BUFFER_SIZE]; // Производитель процессора.
    char cpu_cores[BUFFER_SIZE]; // Количество ядер процессора.
    char cpu_family[BUFFER_SIZE]; // Семейство процессора.
    char cpu_microcode[BUFFER_SIZE]; // Версия микрокода процессора.
    char cpu_cache[BUFFER_SIZE]; // Размер кэша процессора.
    char cpu_bogomips[BUFFER_SIZE]; // Псевдо-метрика производительности процессора.
    char cpu_addres_size[BUFFER_SIZE]; // Размер адресного пространства процессора.
}; 

typedef struct {
    char name[DEVICE_NAME_SIZE]; // Имя устройства.
    char rm[DEVICE_RM_SIZE]; // Указывает, является ли устройство съемным.
    float size; // Размер устройства в мегабайтах.
    char ro[DEVICE_RM_SIZE]; // Указывает, является ли устройство только для чтения.
    char mount_point[DEVICE_MOUNT_POINT_SIZE]; // Точка монтирования устройства.
} DeviceInfo;

typedef struct {
    char name[MODULE_NAME_SIZE]; // Имя модуля.
    char size[MODULE_SIZE_SIZE]; // Размер модуля.
    char used_by[MODULE_USED_BY_SIZE]; // Какие процессы или модули используют данный модуль.
} KernelModule;

void get_proc_info(); // Прототип функции для получения информации о процессах.
void get_cpu_info(); // Прототип функции для получения информации о процессоре.
void cpu_temp(); // Прототип функции для получения температуры процессора
int fill_cpu(struct cpu_info *cpu); // Прототип функции для заполнения структуры cpu_info информацией о процессоре.
void get_system_info(); // Прототип функции для получения общей информации о системе.
void get_usb_info(); // Прототип функции для получения информации о подключенных USB устройствах.
void get_loop_info(); // Прототип функции для получения информации о жестких дисках.
void get_information(); // Прототип функции для получения информации об утилите.
void get_network_info(); // Прототип функции для получения информации о сетевых соединениях.
void kernel_info(); // Прототип функции для получения информации о ядре.
void get_modules_info(); // Прототип функции для получения информации о модулях ядра.
long uptime(); // Прототип функции для получения времени работы системы.
int compare(const void *a, const void *b); // Прототип функции для сравнения устройств.
void activate(GtkApplication *app, gpointer user_data); // Прототип функции для создания и отображения главного окна приложения GTK.
void get_cpu_load();
void print_file_content(const char *filename);
#endif 
