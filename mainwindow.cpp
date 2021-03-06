#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "qdebug.h"
#include "QString"
#include "QKeyEvent"
#include <QTableWidgetItem>
#include <QAbstractItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget_2->setRowCount(256);
    ui->tableWidget_2->setColumnCount(16);


    for(int i=0; i<16;i++){
        ui->tableWidget_2->setColumnWidth(i,51);
    }
    for (int var = 0; var < 256; var++) {
        ui->tableWidget_2->setRowHeight(var,20);
    }

    initialize();


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initialize(){
    ui->label->setText("");
    for (int var = 0; var < 4096; var++) {
        memory[var] = 0;
    }

    pc = 0;
    flags = 0;
    x = 0;

    for(int i = 0; i< 8 ; i++){
        reg[i] = 0;
    }


    QAbstractItemModel *model = ui->tableWidget_2->model();
    QStringList labels;
    QString hexvalues;

    for(int i = 0; i<256; i++){
        QVariant data = model->headerData(i, Qt::Vertical);
        hexvalues = QString("%1").arg(i);
        hexvalues.setNum(i, 16);
         labels << hexvalues;
    }
    ui->tableWidget_2->setVerticalHeaderLabels(labels);
    for(int i = 0; i<16; i++){
        QVariant data = model->headerData(i, Qt::Vertical);
        hexvalues = QString("%1").arg(i);
        hexvalues.setNum(i, 16);
        labels << hexvalues;
    }


    ui->tableWidget_2->setHorizontalHeaderLabels(labels);


}

/*
void MainWindow::keyReleaseEvent(QKeyEvent * a){
    if(memory[80] == 0){
        memory[80] = 1;
        memory[81] = a->nativeScanCode();
        updateScreen();
    }
    else
        qDebug("%d", memory[80]);
}

Burası alakalı değil
*/


void MainWindow::on_pushButton_clicked()
{
    initialize();
    FILE *intructions;
    QString filepath1 = ui->textEdit->toPlainText();
    QByteArray ba = filepath1.toLatin1();
    intructions = fopen(ba.data(), "r");
    if(intructions == NULL){
        ui->label->setText("Invalid adress");
        return;
    }
    while(!feof(intructions)){
    fscanf(intructions, "%4x\n", &memory[x]);
        x++;
    };
    updateScreen();
}





void MainWindow::updateScreen(){

     ui->tableWidget->setItem(-1,1,new QTableWidgetItem(QString::number(pc)));

    ui->tableWidget->setItem(8,1,new QTableWidgetItem(QString::number(flags)));

    for (int i = 0; i < 8; ++i) {
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(reg[i])));
    }


   // ui->tableWidget->item(0,5)->setBackgroundColor(Qt::yellow);

    int xx = 0;
    QString hexvalue;
    QFont fnt;
            fnt.setPointSize(10);
            fnt.setFamily("Arial");

    for(int i = 0; i<256; i++){
        for(int j = 0; j<16; j++){
            hexvalue.setNum(memory[xx],16);
            ui->tableWidget_2->setItem(i,j, new QTableWidgetItem(hexvalue));
            ui->tableWidget_2->item(i,j)->setFont(fnt);
            if(xx == pc){
                ui->tableWidget_2->item(i,j)->setBackground(Qt::red);
            }
            xx++;
        }
    }

}
void MainWindow::on_pushButton_2_clicked()
{
        if(memory[pc] > 0x0fff && memory[pc] < 0x2000 ){
            // ldi
            reg[memory[pc]&0x0003] = memory[pc + 1];
            pc = pc + 2;
        }
        else if(memory[pc] > 0x1fff && memory[pc] < 0x3000 ){
           // ld
            int y = reg[(memory[pc]&0x0018)>>3];
            reg[memory[pc]&0x0003] = memory[y];
            pc++;
        }
        else if(memory[pc] > 0x2fff && memory[pc] < 0x4000 ){
          // st
            memory[reg[(memory[pc]&0x0018)>>3]] = reg[(memory[pc]&0x00c0)>>6];
            pc++;
        }
        else if(memory[pc] > 0x3fff && memory[pc] < 0x5000 ){
            // jz
            if(flags == 1){
                if((memory[pc] - 0x4000) > 0x6ff){
                    pc = (-1) * ((((~(memory[pc] - 0x4000))&0xfff) + 1)) + 1 + pc;
                }
                else{
                    pc = ((memory[pc] - 0x4000)&0xfff) + 1 + pc;
                }
            }
            else
                pc++;
        }
        else if(memory[pc] > 0x4fff && memory[pc] < 0x6000 ){
           // jump
            if((memory[pc] - 0x5000) > 0x6ff){
                pc = (-1) * ((((~(memory[pc] - 0x5000))&0xfff) + 1)) + 1 + pc;
            }
            else{
                printf("%04x \n%04x \n %04x\n" , memory[pc] , (memory[pc] - 0x5000), ((memory[pc] - 0x5000)&0xfff)) ;

                pc = ((memory[pc] - 0x5000)&0xfff) + 1 + pc;
            }
        }
        else if(memory[pc] > 0x6fff && memory[pc] < 0x8000 ){
            //0x7000
            int temp = (memory[pc]>>8)&0x000f;
            qDebug("%d", temp);
            if(temp == 0){
                // add
                reg[memory[pc]&0x0003] = reg[(memory[pc]>>3)&0x0003] + reg[((memory[pc])>>6)&0x0003];
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
                }
            else if(temp == 1){
                // sub
                reg[memory[pc]&0x0003] = reg[(memory[pc]>>3)&0x0003] + reg[((memory[pc])>>6)&0x0003];
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 2){
                reg[memory[pc]&0x0003] = ((reg[(memory[pc]>>3)&0x0003])&(reg[((memory[pc])>>6)&0x0003]));

                // and
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 3){
                // or
                reg[memory[pc]&0x0003] = ((reg[(memory[pc]>>3)&0x0003])|(reg[((memory[pc])>>6)&0x0003]));
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 4){
                // xor
                reg[memory[pc]&0x0003] = ((reg[(memory[pc]>>3)&0x0003])^(reg[((memory[pc])>>6)&0x0003]));
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 5){
                //not
               reg[memory[pc]&0x0003] = ~(reg[(memory[pc]>>3)&0x0003]);
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 6){
                reg[memory[pc]&0x0003] = reg[(memory[pc]>>3)&0x0003];
            }
            else if(temp == 7){
                reg[memory[pc]&0x0003]++;
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            else if(temp == 8){

                reg[memory[pc]&0x0003]--;
                if(reg[memory[pc]&0x0003] == 0)
                    flags = 1;
                else
                    flags = 0;
            }
            pc++;
        }
        else
            pc++;
     updateScreen();

}



// 80 olayı halledilecek!!!
