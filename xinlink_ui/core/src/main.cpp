#include "mainwindow.h"

#include <QApplication>

//extern "C"{
#include <core.h>
#include <instruction.h>
#include <network.h>
#include <dsp.h>
//}

int main(int argc, char *argv[])
{

    if(xinlink_init())printf("xinlink init success!\n");
    dsp_init();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
