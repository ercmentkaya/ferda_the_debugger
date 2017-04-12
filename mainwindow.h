#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int pc;//this variable is program counter that is points current instruction in memory
    int x;//
    int memory[4096];//The array represents our memory
    int reg[8];//The array represents our registers
    double flags;//The variable represents zero flag
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //prototype of functions

    void on_pushButton_clicked();
    void updateScreen();
    void on_pushButton_2_clicked();
    void initialize();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
