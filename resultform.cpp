#include "resultform.h"
#include "ui_resultform.h"
#include "mainwindow.h"
#include <iostream>
#include <math.h>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMenuBar>

using namespace std;

char sci = 'g';
int dpi = 96;
int tileWidth = 512;
int zoomLevels = 20;
float mapWidth = 0;
float mapHeight = 0;
float boundingBox[4];
float** mapData;

resultForm::resultForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::resultForm)
{
    ui->setupUi(this);
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->sciBox,SIGNAL(clicked()),this,SLOT(setTable()));
    connect(ui->googleButton,SIGNAL(clicked()),this,SLOT(compute()));
    connect(ui->sampleButton,SIGNAL(clicked()),this,SLOT(compute()));
}

void resultForm::initialize(float bb[4], int mdpi, int mpix, int mzoom, float mwidth, float mheight)
{
    int i;

    mapData = new float*[mzoom];
    for(i=0;i<mzoom;i++)
    {
        mapData[i] = new float[COLS];
    }

    boundingBox[0]=bb[0]; boundingBox[1]=bb[1]; boundingBox[2]=bb[2]; boundingBox[3]=bb[3];
    dpi = mdpi;
    tileWidth = mpix;
    zoomLevels = mzoom;
    mapWidth = mwidth;
    mapHeight = mheight;

    /* Set up the form */
    ui->tableWidget->move(0,0);
    ui->tableWidget->setColumnCount(COLS);
    ui->tableWidget->setRowCount(zoomLevels);
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resize(WIDTH,HEIGHT(zoomLevels));
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
                         "  |  Tile Width (pixels) = " + QString::number(tileWidth)
                         );
    this->setFixedSize(WIDTH,HEIGHT(zoomLevels)+40);
    ui->saveButton->move(WIDTH/2-ui->saveButton->width()/2,HEIGHT(zoomLevels)+5);
    ui->sciBox->move(WIDTH/2 - 200,HEIGHT(zoomLevels)+5);
    ui->googleButton->move(WIDTH/2-400,HEIGHT(zoomLevels)+5);
    ui->sampleButton->move(WIDTH/2-400,HEIGHT(zoomLevels)+25);
    ui->sampleButton->setChecked(true);

    /* Calculate the data */
    compute();
}

/* Save the current results to a file */
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
            out << mapData[i][j];
            if(j!=COLS-1) out << ";";
        }
        out << "\n";
    }
    file.close();
}

/* Sets the data in the table widget */
void resultForm::setTable()
{
    if(ui->sciBox->isChecked()) sci = 'g';
    else sci = 'f';

    int i,j;
    for(i=0;i<zoomLevels;i++)
    {
        for(j=0;j<COLS;j++)
        {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(mapData[i][j],sci,PRECISION)));
        }
    }
}

void resultForm::compute()
{
    int i;

    float phi = boundingBox[0] - (boundingBox[0]-boundingBox[1])/2;

    for(i=0;i<zoomLevels;i++)
    {
        /*Zoom Level*/
        mapData[i][0]=i;

        /*Num Cols*/
        mapData[i][2]=pow(2,i);

        /*Num Rows*/
        if(ui->sampleButton->isChecked()){
            if(i>0) mapData[i][3]= mapData[i][2]/2;
            else mapData[i][3] = 1;
        }
        else
            mapData[i][3] = mapData[i][2];

        /*Num Tiles*/
        mapData[i][1] = mapData[i][2] * mapData[i][3];

        /*Degrees Long per tile */
        mapData[i][4] = mapWidth / mapData[i][2];

        /*Degrees lat per tile */
        mapData[i][5] = mapHeight / mapData[i][3];

        /*Degrees per pixel*/
        mapData[i][6] = mapWidth / (pow(2,i) * tileWidth);

        /*Ground resolution at latitude phi*/
        mapData[i][7] = cos(phi*(M_PI/180))*2*M_PI*
                (EARTH_CIRCUM/(512*pow(2,i)));

        /* Map Scale */
        mapData[i][8] = (dpi/IN_TO_M)*mapData[i][7];
    }

    setTable();
}

/* Default destructor */
resultForm::~resultForm()
{
    delete ui;
}
