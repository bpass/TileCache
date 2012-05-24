#include "resultform.h"
#include "ui_resultform.h"
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMenuBar>

using namespace std;

#define COLS 9
#define HEIGHT(z) ((z+1)*21+3)
#define WIDTH 901

bool sci = true;

resultForm::resultForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::resultForm)
{
    ui->setupUi(this);
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(save()));
}

void resultForm::initialize(float bb[4], int dpi, int pix, int zoom, float arr[][COLS])
{
    ui->tableWidget->move(0,0);
    ui->tableWidget->setColumnCount(COLS);
    ui->tableWidget->setRowCount(zoom);
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resize(WIDTH,HEIGHT(zoom+1));
    ui->tableWidget->horizontalHeader()->setResizeMode (QHeaderView::Fixed);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Zoom\nLevel" <<
                                               "Number of \n Tiles" <<
                                               "Number of \n Columns" <<
                                               "Number of \n Rows" <<
                                               "Degrees \n Longitude \n per Tile" <<
                                               "Degrees \n Latitude \n per Tile" <<
                                               "Degrees \n per Pixel" <<
                                               "Ground Resolution \n (meters/pixel)" <<
                                               "Map Scale \n 1:X \n at " + QString::number(dpi) + " DPI");
    this->setWindowTitle("Results: High Lat = " + QString::number(bb[0]) +
                         "  |  Low Lat = " + QString::number(bb[1]) +
                         "  |  High Long = " + QString::number(bb[2]) +
                         "  |  Low Long = " + QString::number(bb[3]) +
                         "  |  DPI = " + QString::number(dpi) +
                         "  |  Tile Width (pixels) = " + QString::number(pix)
                         );
    this->setFixedSize(WIDTH,HEIGHT(zoom+1)+40);
    ui->saveButton->move(WIDTH/2-ui->saveButton->width()/2,HEIGHT(zoom+1)+5);

    /*
    QMenuBar* menu = new QMenuBar(this);
    menu->addMenu("File");
    menu->addAction("Save");
    menu->addMenu("Help");
    menu->addAction("Some help");
    */

    int i,j;
    for(i=0;i<zoom;i++)
    {
        for(j=0;j<COLS;j++)
        {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(arr[i][j],'g')));
        }
    }
}

void resultForm::save()
{
    QFile file(QFileDialog::getSaveFileName());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "Zoom Level;Number of Tiles;Number of Columns;Number of Rows;" <<
           "Degrees Longitude per Pixel;Degrees Latitude per Pixel;" <<
           "Degrees per Pixel;Ground Resolution;Map Scale 1:X (meters/pixel)\n";

    int i,j;
    for(i=0;i<ui->tableWidget->rowCount();i++)
    {
        for(j=0;j<COLS;j++)
        {
            out << ui->tableWidget->item(i,j)->text();
            if(j!=COLS-1) out << ";";
        }
        out << "\n";
    }
    file.close();
}

resultForm::~resultForm()
{
    delete ui;
}
