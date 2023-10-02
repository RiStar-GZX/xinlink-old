#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <core.h>
#include <instruction.h>
#include <network.h>
#include <dsp.h>

#include <qdebug.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    extern XLll * core_ll;
    XLll_member *member_now=core_ll->head;
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core=(XLcore*)member_now->data;
        ui->listWidget->addItem(core->name);
        member_now=member_now->next;
    }

    extern XLll * event_ll;
    member_now=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event=(XLevent*)member_now->data;
        ui->listWidget_2->addItem(event->sign.name);
        member_now=member_now->next;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setS(const QString &newS)
{
    s = newS;
}




void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    const QString str=item->text();
    qDebug()<<"str:"<<str;
    this->s=str;
}




void MainWindow::on_lineEdit_returnPressed()
{
    this->ins=ui->lineEdit->text();
    qDebug()<<"ins:"<<this->ins;
    QByteArray ba=this->s.toUtf8();
    char * s=ba.data();
    QByteArray bains=this->ins.toUtf8();
    char * ins=bains.data();
    event_id_t id=event_get_by_signname(s);
    if(id<=0)return;
    //XLevent * event=event_get_by_id(id);

    XLsource receiver,sender;
    receiver.mode=EVENT_ID;
    receiver.id=id;
    receiver.net=network_get_local_info();
    sender.mode=ACCESS;
    sender.id=11;
    if(network_send_ins(&sender,&receiver,OPERATE_CODE_CONNECT,ins)>=0)printf("Success!\n");
    else printf("Fail!\n");
}

