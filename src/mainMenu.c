    #include "mainMenu.h"

    int fill_cpu(struct cpu_info *cpu){     //получить информацио о процессоре
        char buffer[BUFFER_SIZE];
        FILE *fp = fopen("/proc/cpuinfo", "r");
        if (fp == NULL) {
            perror("Не удалось открыть файл /proc/cpuinfo\n");
            return 1;
        }

        while (fgets(buffer, sizeof(buffer), fp)) {     
            if (strncmp(buffer, "model name", 10) == 0) {
                strcpy(cpu->cpu_product, buffer);
            }
            else if (strncmp(buffer, "vendor_id", 9) == 0) {    
                strcpy(cpu->cpu_vendor, buffer);
            }
            else if (strncmp(buffer, "cpu cores", 9) == 0) {    
                strcpy(cpu->cpu_cores, buffer);
            }
            else if (strncmp(buffer, "cpu family", 9) == 0) {   
                strcpy(cpu->cpu_family, buffer);
            }
            else if (strncmp(buffer, "microcode", 9) == 0) {    
                strcpy(cpu->cpu_microcode, buffer);
            }
            else if (strncmp(buffer, "cache size", 10) == 0) {  
                strcpy(cpu->cpu_cache, buffer);
            }
            else if (strncmp(buffer, "bogomips", 8) == 0) {     
                strcpy(cpu->cpu_bogomips, buffer);
            }
            else if (strncmp(buffer, "address sizes", 13) == 0) {   
                strcpy(cpu->cpu_addres_size, buffer);
            }
            else if(strncmp(buffer, "\n", 1) == 0)
                break;
        }

        if (ferror(fp)) {
            perror("Ошибка при чтении файла /proc/cpuinfo");
            fclose(fp);
            return 1;
        }

        fclose(fp);
        return 0;
    }
    void get_cpu_load() {
        FILE* file = fopen("/proc/stat", "r");
        if (file == NULL) {
            perror("Не удалось открыть файл /proc/stat\n");
            return;
        }

        char buffer[128];
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

        fgets(buffer, sizeof(buffer), file);
        sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

        unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
        unsigned long long total_idle = idle + iowait;
        unsigned long long total_usage = total - total_idle;

        printf("Загруженность процессора: %.2f%%\n", (double)total_usage / total * 100);

        fclose(file);
    }

    void get_cpu_info(){ //информация о процессоре
        system("clear");       
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
        get_cpu_load();
    }

    void get_proc_info() {
        system("clear"); 
        DIR *dir;
        struct dirent *entry; // один элемент в каталоге /proc.
        char path[BUFFER_SIZE]; // для хранения пути к файлам в каталоге /proc.
        char cmdline[BUFFER_SIZE]; // для хранения содержимого файла /proc/PID/cmdline.
        char time_str[TIME_SIZE]; // для хранения времени выполнения процесса в виде строки.
        char *cmd; // для разбора командной строки процесса.
        char status[BUFFER_SIZE]; // для хранения содержимого файла /proc/PID/status.
        uid_t uid; // Целочисленное значение, представляющее идентификатор пользователя.
        dir = opendir("/proc");
        if (dir == NULL) {
            perror("Ошибка открытия /proc");
            return;
        }
        printf("%-20s %-5s %-10s %-10s %-s\n", "USER", "PID", "TIME", "STARTTIME", "COMMAND"); 
        while ((entry = readdir(dir)) != NULL) {
            // Проверяем, является ли имя каталога числами (PID) (гарант того что обрабатываются только директории процессов)
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
                            char stat[BUFFER_SIZE];
                            if (fgets(stat, sizeof(stat), fp) != NULL) {
                                sscanf(stat, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", time_str);
                                unsigned long time_ticks = atol(time_str);
                                unsigned long seconds = time_ticks / sysconf(_SC_CLK_TCK);
                                unsigned long hours = seconds / 3600;
                                seconds %= 3600;
                                unsigned long minutes = seconds / 60;
                                seconds %= 60;
                                printf("%02lu:%02lu:%02lu   ", hours, minutes, seconds);
                                unsigned long starttime;
                                sscanf(stat, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu", &starttime);
                                starttime = starttime / sysconf(_SC_CLK_TCK);
                                time_t now = time(NULL);
                                time_t start = now - uptime() + starttime;
                                struct tm *start_tm = localtime(&start);
                                printf("%02d:%02d:%02d   ", start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec);
                            } else {
                                printf("%-10s ", "?");
                            }
                            fclose(fp); 
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
            } else {
                fclose(fp);
                return -1; 
            }
        } else {
            perror("Ошибка открытия файла /proc/uptime");
            return -1;
        }
    }


    void cpu_temp(){ //температура процессора
        FILE* temp_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");

        if (temp_file == NULL) {
            perror("Не удалось открыть \"/sys/class/thermal/thermal_zone0/temp\"\n");
            return;
        }

        int temp_raw;
        fscanf(temp_file, "%d", &temp_raw);
        fclose(temp_file);
        float temp_c = (float) temp_raw / 1000; 
        const char* color;

        if (temp_c < 60) {
            color = GREEN; 
        } else if (temp_c < 70) {
            color = YELLOW; 
        } else {
            color = RED;
        }
        
        printf("%sТемпература процессора: %.1f C\n%s", color, temp_c, RESET);
    }

    void get_system_info() //информация о системе
    {
        system("clear");
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
        system("clear"); 
        FILE *fp;
        char line[MODULE_NAME_SIZE];
        KernelModule kernel_module;

        printf(ANSI_BOLD GREEN "Загруженные модули:\n" RESET);
        printf(ANSI_BOLD RED "%-30s %-12s %-10s\n", "Имя модуля", "            Размер", " Используется" RESET);
        
        fp = fopen("/proc/modules", "r");
        if (fp == NULL) {
            perror("Не удалось открыть /proc/modules");
            return;
        }
        
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%255s %255s %*255s %255s", kernel_module.name, kernel_module.size, kernel_module.used_by) == 3) {
                printf("%-30s %10s %10s\n", kernel_module.name, kernel_module.size, kernel_module.used_by);
            } else {
                printf("Ошибка при чтении модуля: %s", line);
            }
        }
        fclose(fp);
    }

    void kernel_info() { //информация о ядре
        system("clear");    
        struct utsname buffer;

        if (uname(&buffer) != 0) {
            perror("Не удалось получить информацию\n");
            return;
        }

        printf("Operating System: %s\n", buffer.sysname);
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

            unsigned char string[MODULE_NAME_SIZE];
            if (desc.iProduct != 0) {
                r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
                if (r < 0) {
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
        DeviceInfo devices[HDD_SIZE];
        int device_count = 0;

        if ((dir = opendir(path)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                char *device_name = ent->d_name;
                if(strstr(device_name, "loop") != NULL || strstr(device_name, "sd") != NULL) {

                    strncpy(devices[device_count].name, device_name, sizeof(devices[device_count].name) - 1);
                    char rm_path[HDD_SIZE];
                    sprintf(rm_path, "%s%s/removable", path, device_name);

                    FILE *file = fopen(rm_path, "r");
                    if (file != NULL) {
                        fgets(devices[device_count].rm, sizeof(devices[device_count].rm), file);
                        devices[device_count].rm[strcspn(devices[device_count].rm, "\n")] = 0; 
                        fclose(file);
                    }

                    char size_path[HDD_SIZE];
                    sprintf(size_path, "%s%s/size", path, device_name);

                    file = fopen(size_path, "r");
                    if (file != NULL) {
                        char size[HDD_SIZE];
                        fgets(size, sizeof(size), file);
                        size[strcspn(size, "\n")] = 0;  
                        unsigned long long blocks = strtoull(size, NULL, 10);
                        devices[device_count].size = (blocks * 512) / (1024.0 * 1024.0);  
                        fclose(file);
                    }

                    char ro_path[HDD_SIZE];
                    sprintf(ro_path, "%s%s/ro", path, device_name);

                    file = fopen(ro_path, "r");
                    if (file != NULL) {
                        fgets(devices[device_count].ro, sizeof(devices[device_count].ro), file);
                        devices[device_count].ro[strcspn(devices[device_count].ro, "\n")] = 0; 
                        fclose(file);
                    }
                    // Получение точек монтирования
                    strncpy(devices[device_count].mount_point, "N/A", sizeof(devices[device_count].mount_point) - 1);

                    FILE *mounts = fopen("/proc/mounts", "r");
                    if (mounts != NULL) {
                        char line[HDD_SIZE];
                        while (fgets(line, sizeof(line), mounts)) {
                            char dev[HDD_SIZE], mp[HDD_SIZE];
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

    void print_file_content(const char *filename) {
        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            perror("Ошибка при открытии файла");
            return;
        }

        char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file)) {
            printf("%s", buffer);
        }

        fclose(file);
    }

    void get_information(){
        system("clear");
        print_file_content("/home/victor/Рабочий стол/course work/text.txt");
        printf("Разработчик: Приловский Виктор Витальевич\nСтудент группы 250503.\n");
        
    }
    
    void get_network_info() {
        system("clear");
        system("ss -tulnp");
    }

    // Функция для создания главного окна приложения
    void activate(GtkApplication *app, gpointer user_data)
    {
        
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
