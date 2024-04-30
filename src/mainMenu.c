#include "mainMenu.h"

int fill_cpu(struct cpu_info *cpu){     //получить информацио о процессоре
    char buffer[1024];
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        perror("Не удалось открыть файл /proc/cpuinfo\n");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {     //модель
        if (strncmp(buffer, "model name", 10) == 0) {
            strcpy(cpu->cpu_product, buffer);
        }
        else if (strncmp(buffer, "vendor_id", 9) == 0) {    //происзводитель
            strcpy(cpu->cpu_vendor, buffer);
        }
        else if (strncmp(buffer, "cpu cores", 9) == 0) {    //кол-во ядер
            strcpy(cpu->cpu_cores, buffer);
        }
        else if (strncmp(buffer, "cpu family", 9) == 0) {   //семья процессора
            strcpy(cpu->cpu_family, buffer);
        }
        else if (strncmp(buffer, "microcode", 9) == 0) {    //микрокод
            strcpy(cpu->cpu_microcode, buffer);
        }
        else if (strncmp(buffer, "cache size", 10) == 0) {  //размер кеша
            strcpy(cpu->cpu_cache, buffer);
        }
        else if (strncmp(buffer, "bogomips", 8) == 0) {     //скорость исполнения
            strcpy(cpu->cpu_bogomips, buffer);
        }
        else if (strncmp(buffer, "address sizes", 13) == 0) {   //размер адресов
            strcpy(cpu->cpu_addres_size, buffer);
        }
        else if(strncmp(buffer, "\n", 1) == 0)
            break;
    }

    fclose(fp);
    return 0;
}

void get_cpu_info(){ 
    system("clear");       //информация о процессоре
    struct cpu_info cpu;
    if(fill_cpu(&cpu))
        return;

    printf("%s", cpu.cpu_product);
    printf("%s", cpu.cpu_vendor);
    printf("%s", cpu.cpu_family);
    printf("%s", cpu.cpu_cores);
    printf("%s", cpu.cpu_microcode);
    printf("%s", cpu.cpu_cache);
    printf("%s", cpu.cpu_bogomips);
    printf("%s", cpu.cpu_addres_size);
}

void get_proc_info() {
    system("clear"); 
    DIR *dir;
    struct dirent *entry;
    char path[1024];
    char cmdline[1024];
    char time_str[32]; // Для TIME
    char *cmd;
    char status[1024];
    uid_t uid;
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Ошибка открытия /proc");
        return;
    }
    printf("%-20s %-5s %-10s %-10s %-s\n", "USER", "PID", "TIME", "STARTTIME", "COMMAND"); 
    while ((entry = readdir(dir)) != NULL) {
        // Проверяем, является ли имя каталога числами (PID)
        if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
            snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name);
            FILE *fp = fopen(path, "r");
            if (fp != NULL) {
                if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                    cmdline[strcspn(cmdline, "\n")] = 0;
                    cmd = strtok(cmdline, " ");
                    fclose(fp);
                    snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
                    fp = fopen(path, "r");
                    if (fp != NULL) {
                        while (fgets(status, sizeof(status), fp) != NULL) {
                            if (strncmp(status, "Uid:", 4) == 0) {
                                sscanf(status, "Uid:\t%*u %u", &uid);
                                struct passwd *pw = getpwuid(uid);
                                if (pw != NULL) {
                                    printf("%-20s ", pw->pw_name);
                                } else {
                                    printf("%-20s ", "?");
                                }
                                break;
                            }
                        }
                        fclose(fp);
                    } else {
                        printf("%-20s ", "?");
                    }
                    // Выводим PID
                    printf("%-5s ", entry->d_name);
                    
                    // Получаем TIME
                    snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
                    fp = fopen(path, "r");
                    if (fp != NULL) {
                        // Считываем строку из файла
                        char stat[1024];
                        if (fgets(stat, sizeof(stat), fp) != NULL) {
                            // Извлекаем TIME из строки
                            sscanf(stat, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", time_str);
                            // Преобразуем "такты" в секунды
                            unsigned long time_ticks = atol(time_str);
                            unsigned long seconds = time_ticks / sysconf(_SC_CLK_TCK);
                            // Вычисляем часы, минуты и секунды
                            unsigned long hours = seconds / 3600;
                            seconds %= 3600;
                            unsigned long minutes = seconds / 60;
                            seconds %= 60;
                            printf("%02lu:%02lu:%02lu   ", hours, minutes, seconds);

                            // Получаем STARTTIME
                            unsigned long starttime;
                            sscanf(stat, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu", &starttime);
                            // Преобразуем "такты" в секунды
                            starttime = starttime / sysconf(_SC_CLK_TCK);
                            // Получаем текущее время в секундах
                            time_t now = time(NULL);
                            // Вычисляем время запуска процесса
                            time_t start = now - uptime() + starttime;
                            // Преобразуем в формат времени
                            struct tm *start_tm = localtime(&start);
                            printf("%02d:%02d:%02d   ", start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec);
                        } else {
                            printf("%-10s ", "?");
                        }
                        fclose(fp); // Закрываем файл после использования
                    } else {
                        printf("%-10s ", "?");
                    }
                    
                    printf("%s\n", cmd);
                }
            }
        }
    }

    closedir(dir);
}

long uptime() {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp != NULL) {
        double uptime;
        if (fscanf(fp, "%lf", &uptime) == 1) {
            fclose(fp);
            return (long)uptime;
        }
        fclose(fp);
    }
    return 0;
}
void cpu_temp(){    //температура процессора
    FILE* temp_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

    if (temp_file == NULL) {
        perror("Не удалось открыть \"/sys/class/thermal/thermal_zone0/temp\"\n");
        return;
    }

    int temp_raw;
    fscanf(temp_file, "%d", &temp_raw);
    fclose(temp_file);

    float temp_c = (float) temp_raw / 1000; // перевод в цельсии

    // Выбор цвета в зависимости от температуры
    const char* color;
    if (temp_c < 60) {
        color = GREEN; // Зелёный
    } else if (temp_c < 70) {
        color = YELLOW; // Жёлтый
    } else {
        color = RED; // Красный
    }

    printf("%sТемпература процессора: %.1f C\n%s", color, temp_c, RESET); // Вывод результата с выбранным цветом
}

void get_system_info()      //информация о системе
{
    system("clear");
    //Получаем информацию о системе
    struct utsname uname_data;
    if (uname(&uname_data) == -1) {
        perror("uname");
        return;
    }
    struct sysinfo sysinfo_data;
    if (sysinfo(&sysinfo_data) == -1) {
        perror("sysinfo");
        return;
    }

    char buffer[BUFFER_SIZE];
    FILE* fp = popen("cat /proc/cpuinfo | grep 'model name' | uniq", "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        perror("fgets");
        pclose(fp);
        return;
    }
    pclose(fp);

    // Выводим информацию на экран
    printf("Operating system: %s %s\n", uname_data.sysname, uname_data.release);
    printf("CPU: %s\n", buffer);
    cpu_temp();
    printf("Machine type: %s\n", uname_data.machine);
    printf("User ID: %d\n", getuid());
    printf("Group ID: %d\n", getgid());
    printf("Effective user ID: %d\n", geteuid());
    printf("Effective group ID: %d\n", getegid());
    printf("Number of processors: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf("Total RAM: %ld MB\n", sysinfo_data.totalram / 1024 / 1024);
    printf("Free RAM: %ld MB\n", sysinfo_data.freeram / 1024 / 1024);
}


void get_modules_info() {
    system("clear"); // Очистка экрана
    FILE *fp;
    char line[256];
    KernelModule kernel_module;

    // Выводим заголовки столбцов с использованием ANSI escape кодов для жирного шрифта и зеленого цвета
    printf(ANSI_BOLD GREEN "Загруженные модули:\n" RESET);
    printf(ANSI_BOLD RED "%-30s %-12s %-10s\n", "Имя модуля", "            Размер", " Используется" RESET);

    // Открываем файл со списком загруженных модулей
    fp = fopen("/proc/modules", "r");
    if (fp == NULL) {
        perror("Не удалось открыть /proc/modules");
        return;
    }

    // Считываем построчно файл и выводим информацию о модулях
    while (fgets(line, sizeof(line), fp)) {
        // Разбиваем строку на токены с помощью функции sscanf
        if (sscanf(line, "%255s %255s %*255s %255s", kernel_module.name, kernel_module.size, kernel_module.used_by) == 3) {
            // Выводим отформатированную информацию о модуле
            printf("%-30s %10s %10s\n", kernel_module.name, kernel_module.size, kernel_module.used_by);
        } else {
            printf("Ошибка при чтении модуля: %s", line);
        }
    }
    fclose(fp);
}

void kernel_info() {
    system("clear");    //информация о ядре
    struct utsname buffer;
    if (uname(&buffer) != 0) {      //заполняем структуру
        perror("Не удалось получить информацию\n");
        return;
    }

    printf("Operating System: %s\n", buffer.sysname);   //вывод информации
    printf("Node Name: %s\n", buffer.nodename);
    printf("Kernel Release: %s\n", buffer.release);
    printf("Kernel Version: %s\n", buffer.version);
    printf("Machine Hardware Name: %s\n", buffer.machine);
}

void get_usb_info() {
    system("clear"); 
    libusb_context *ctx = NULL;
    libusb_device **devs;
    ssize_t cnt;
    int r, i;

    r = libusb_init(&ctx);
    if (r < 0) {
        perror("Не удалось инициализировать libusb");
        return;
    }

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        perror("Не удалось получить список устройств");
        return;
    }

    for (i = 0; i < cnt; i++) {
        libusb_device *dev = devs[i];
        struct libusb_device_descriptor desc;

        r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            perror("Не удалось получить дескриптор устройства");
            continue;
        }

        libusb_device_handle *handle = NULL;
        r = libusb_open(dev, &handle);
        if (r != 0) {
            perror("Не удалось открыть устройство");
            continue;
        }

        unsigned char string[256];
        // Attempt to get the product string descriptor
        if (desc.iProduct != 0) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
            if (r < 0) {
                // Fallback to the manufacturer string descriptor if product string fails
                r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
                if (r < 0) {
                    perror("Не удалось получить описание устройства");
                } else {
                    printf("Bus %03d Device %03d: ID %04x:%04x Device: %s\n", 
                           libusb_get_bus_number(dev), 
                           libusb_get_device_address(dev), 
                           desc.idVendor, desc.idProduct, string);
                }
            } else {
                printf("Bus %03d Device %03d: ID %04x:%04x Device: %s\n", 
                       libusb_get_bus_number(dev), 
                       libusb_get_device_address(dev), 
                       desc.idVendor, desc.idProduct, string);
            }
        } else {
            printf("Bus %03d Device %03d: ID %04x:%04x Device: Unknown\n", 
                   libusb_get_bus_number(dev), 
                   libusb_get_device_address(dev), 
                   desc.idVendor, desc.idProduct);
        }

        libusb_close(handle);
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
}

int compare(const void *a, const void *b) {
    DeviceInfo *deviceA = (DeviceInfo *)a;
    DeviceInfo *deviceB = (DeviceInfo *)b;
    // Сравнение префиксов
    int prefix_cmp = strncmp(deviceA->name, deviceB->name, 4);
    if (prefix_cmp != 0) {
        return prefix_cmp;
    }
    // Сравнение числовых частей как целых чисел
    int numA = atoi(deviceA->name + 4);
    int numB = atoi(deviceB->name + 4);
    return numA - numB;
}

void get_loop_info() {
    system("clear"); 
    DIR *dir;
    struct dirent *ent;
    char path[] = "/sys/block/";

    DeviceInfo devices[256];
    int device_count = 0;

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char *device_name = ent->d_name;
            if(strstr(device_name, "loop") != NULL || strstr(device_name, "sd") != NULL) {
                strncpy(devices[device_count].name, device_name, sizeof(devices[device_count].name) - 1);

                char rm_path[256];
                sprintf(rm_path, "%s%s/removable", path, device_name);
                FILE *file = fopen(rm_path, "r");
                if (file != NULL) {
                    fgets(devices[device_count].rm, sizeof(devices[device_count].rm), file);
                    devices[device_count].rm[strcspn(devices[device_count].rm, "\n")] = 0;  // Удаление символа новой строки
                    fclose(file);
                }

                char size_path[256];
                sprintf(size_path, "%s%s/size", path, device_name);
                file = fopen(size_path, "r");
                if (file != NULL) {
                    char size[256];
                    fgets(size, sizeof(size), file);
                    size[strcspn(size, "\n")] = 0;  // Удаление символа новой строки
                    unsigned long long blocks = strtoull(size, NULL, 10);
                    devices[device_count].size = (blocks * 512) / (1024.0 * 1024.0);  // Преобразование в мегабайты
                    fclose(file);
                }

                char ro_path[256];
                sprintf(ro_path, "%s%s/ro", path, device_name);
                file = fopen(ro_path, "r");
                if (file != NULL) {
                    fgets(devices[device_count].ro, sizeof(devices[device_count].ro), file);
                    devices[device_count].ro[strcspn(devices[device_count].ro, "\n")] = 0;  // Удаление символа новой строки
                    fclose(file);
                }

                // Получение точек монтирования
                strncpy(devices[device_count].mount_point, "N/A", sizeof(devices[device_count].mount_point) - 1);
                FILE *mounts = fopen("/proc/mounts", "r");
                if (mounts != NULL) {
                    char line[256];
                    while (fgets(line, sizeof(line), mounts)) {
                        char dev[256], mp[256];
                        if (sscanf(line, "%s %s", dev, mp) == 2) {
                            char *dev_name = strrchr(dev, '/');
                            if (dev_name && strcmp(dev_name + 1, device_name) == 0) {
                                strncpy(devices[device_count].mount_point, mp, sizeof(devices[device_count].mount_point) - 1);
                                devices[device_count].mount_point[sizeof(devices[device_count].mount_point) - 1] = '\0'; // Ensure null-termination
                                break;
                            }
                        }
                    }
                    fclose(mounts);
                }

                device_count++;
            }
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
    }

    qsort(devices, device_count, sizeof(DeviceInfo), compare);

    printf("%-10s %-5s %-10s %-5s %-15s\n", "NAME", "RM", "SIZE (MB)", "RO", "MOUNTPOINTS");
    for (int i = 0; i < device_count; i++) {
        printf("%-10s %-5s %-10.2f %-5s %-15s\n", devices[i].name, devices[i].rm, devices[i].size, devices[i].ro, devices[i].mount_point);
    }
}

void get_information(){     //вывод информации об утилите
    system("clear");
    printf("\n\nРазработчик: Приловский Виктор Витальевич\nСтудент группы 250503.\n");
}

void get_network_info() {
    system("clear");
    system("ss -tulnp");
}
// Функция для создания главного окна приложения
void activate(GtkApplication *app, gpointer user_data)
{
    // Объявление переменных для виджетов окна
    GtkWidget *window;
    GtkWidget *button_proc;
    GtkWidget *button_cpu;
    GtkWidget *button_system;
    GtkWidget *button_usb;
    GtkWidget *button_loop;
    GtkWidget *button_info;
    GtkWidget *modules_info;
    GtkWidget *button_kernel;
    GtkWidget *vbox;
    GtkWidget *button_network;

    // Создание главного окна и задание его параметров
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SICU");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Создание вертикального контейнера для размещения виджетов
    vbox = gtk_box_new(TRUE, 9);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Создание кнопок и подключение к ним функций обработки событий
    button_proc = gtk_button_new_with_label("Информация о работающих процессах");
    button_cpu = gtk_button_new_with_label("Информация о процессоре");
    button_system = gtk_button_new_with_label("Общая информация о системе");
    button_usb = gtk_button_new_with_label("Информация о подключенных устройствах");
    button_loop = gtk_button_new_with_label("Информация о жестких дисках");
    button_kernel = gtk_button_new_with_label("Информация об ядре");
    modules_info = gtk_button_new_with_label("Информация о модулях установленных в ядро");
    button_network = gtk_button_new_with_label("Информация о сетевых соединениях");
    button_info = gtk_button_new_with_label("Информация об утилите");

    g_signal_connect(button_proc, "clicked", G_CALLBACK(get_proc_info), "proc_button");
    g_signal_connect(button_cpu, "clicked", G_CALLBACK(get_cpu_info), "cpu_button");
    g_signal_connect(button_system, "clicked", G_CALLBACK(get_system_info), "system_button");
    g_signal_connect(button_usb, "clicked", G_CALLBACK(get_usb_info), "usb_button");
    g_signal_connect(button_loop, "clicked", G_CALLBACK(get_loop_info), "loop_button");
    g_signal_connect(button_kernel, "clicked", G_CALLBACK(kernel_info), "kernel_button");
    g_signal_connect(modules_info, "clicked", G_CALLBACK(get_modules_info), "modules_button");
    g_signal_connect(button_network, "clicked", G_CALLBACK(get_network_info), "network_button");
    g_signal_connect(button_info, "clicked", G_CALLBACK(get_information), "info_button");

    // Размещение кнопок в контейнере
    gtk_box_pack_start(GTK_BOX(vbox), button_proc, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_cpu, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_system, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_usb, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_loop, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_kernel, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), modules_info, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_network, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_info, TRUE, TRUE, 5);
    

    // Отображение всех виджетов на экране
    gtk_widget_show_all(window);
}