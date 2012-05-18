#include "resultform.h"
#include "ui_resultform.h"
#include <iostream>

#define COLS 9
#define HEIGHT(z) ((z+1)*21+3)
#define WIDTH 901

resultForm::resultForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::resultForm)
{
    ui->setupUi(this);
}

void resultForm::initialize(float bb[4], int dpi, int pix, int zoom, float arr[][COLS])
{
    ui->tableWidget->setColumnCount(COLS);
    ui->tableWidget->setRowCount(zoom);
    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resize(WIDTH,HEIGHT(zoom));
    ui->tableWidget->horizontalHeader()->setResizeMode (QHeaderView::Fixed);
    this->setWindowTitle("Results: High Lat="); /// TODO: Make Title
    this->resize(WIDTH,HEIGHT(zoom));

    int i,j;
    for(i=0;i<zoom;i++)
    {
        for(j=0;j<COLS;j++)
        {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(arr[i][j])));
        }
    }
}

resultForm::~resultForm()
{
    delete ui;
}
