#include "mainMenu.h"
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    // инициализация приложения
    app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Запуск приложениz
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}