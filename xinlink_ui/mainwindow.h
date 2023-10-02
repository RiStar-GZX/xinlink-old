#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString s;
    QString ins;
    void setS(const QString &newS);

private slots:
    void on_listWidget_2_itemClicked(QListWidgetItem *item);


    void on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
