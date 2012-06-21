////////////////////////////////////////////
/* Created by Brian Passuello             */
/* Property of USGS                       */
/* Last edited 06/21/12                   */
////////////////////////////////////////////


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
int precision = 4;
float mapWidth = 0;
float mapHeight = 0;
float boundingBox[4];
float** mapData;

/*
 * Constructor
 */
resultForm::resultForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::resultForm)
{
    ui->setupUi(this);
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->sciBox,SIGNAL(clicked()),this,SLOT(setTable()));
    connect(ui->googleButton,SIGNAL(clicked()),this,SLOT(compute()));
    connect(ui->sampleButton,SIGNAL(clicked()),this,SLOT(compute()));
    connect(ui->precBox,SIGNAL(valueChanged(int)),this,SLOT(compute()));
    connect(ui->scaleButton,SIGNAL(clicked()),this,SLOT(addRow()));
    connect(ui->scaleSaveButton,SIGNAL(clicked()),this,SLOT(saveScale()));
}

/*
 * Creates the window used for displaying results.
 */
void resultForm::initialize(float bb[4], int mdpi, int mpix, int mzoom, float mwidth, float mheight)
{
    int i;

    /* Initializes the 2D array being used to hold cell data */
    mapData = new float*[mzoom];
    for(i=0;i<mzoom;i++)
    {
        mapData[i] = new float[COLS];
    }

    /* Sets local variables */
    boundingBox[0]=bb[0]; boundingBox[1]=bb[1]; boundingBox[2]=bb[2]; boundingBox[3]=bb[3];
    dpi = mdpi;
    tileWidth = mpix;
    zoomLevels = mzoom;
    mapWidth = mwidth;
    mapHeight = mheight;

    /* Set up the main table */
    ui->tableWidget->move(10,0);
    ui->tableWidget->setColumnCount(COLS);
    ui->tableWidget->setRowCount(zoomLevels);
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resize(901,HEIGHT(zoomLevels)+5);
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

    /* Set up the map scale table */
    ui->scaleTable->move(ui->tableWidget->x()+ui->tableWidget->width()+10,0);
    ui->scaleTable->setRowCount(0);
    ui->scaleTable->resizeRowsToContents();
    ui->scaleTable->resize(2*ui->scaleTable->columnWidth(1),ui->tableWidget->height());
    ui->scaleTable->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    /* Set up various buttons */
    ui->saveButton->move(
                ui->tableWidget->x()+ui->tableWidget->width()/2-ui->saveButton->width(),
                HEIGHT(zoomLevels)+20);
    ui->scaleSaveButton->move(
                ui->tableWidget->x()+ui->tableWidget->width()/2,HEIGHT(zoomLevels)+20);

    ui->googleButton->move(ui->tableWidget->x()+10,HEIGHT(zoomLevels)+5);
    ui->sampleButton->move(ui->tableWidget->x()+10,HEIGHT(zoomLevels)+25);

    ui->sciBox->move(ui->tableWidget->x()+110,HEIGHT(zoomLevels)+15);
    ui->precLabel->move(ui->tableWidget->x()+230,HEIGHT(zoomLevels)+15);
    ui->precBox->move(ui->precLabel->x()+ui->precBox->width()+5,HEIGHT(zoomLevels)+15);

    ui->scaleButton->move(ui->scaleTable->x()+ui->scaleTable->width()/2-ui->scaleButton->width()/2,HEIGHT(zoomLevels)+25);
    ui->scaleText->move(ui->scaleTable->x()+ui->scaleTable->width()/2-ui->scaleText->width()/2,HEIGHT(zoomLevels)+5);

    /* Set up the form */
    this->setWindowTitle("Results: Low Lat = " + QString::number(bb[1]) +
                         "  |  High Lat = " + QString::number(bb[0]) +
                         "  |  Low Long = " + QString::number(bb[3]) +
                         "  |  High Long = " + QString::number(bb[2]) +
                         "  |  DPI = " + QString::number(dpi) +
                         "  |  Tile Width (pixels) = " + QString::number(tileWidth)
                         );
    this->setFixedSize(ui->tableWidget->width()+ui->scaleTable->width()+30,HEIGHT(zoomLevels)+50);

    compute();

    /* Adds some default scales */
    addLatRow(24000); addLatRow(25000); addLatRow(48000);
    addLatRow(63000); addLatRow(100000); addLatRow(125000);
    addLatRow(250000); addLatRow(500000); addLatRow(1000000);
    addLatRow(2000000);
}

/*
 * Save the current results to a file
 */
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

/*
 * Saves the data from the map scale table
 */
void resultForm::saveScale(){
    QFile file(QFileDialog::getSaveFileName());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "Map Scale (1:X);Central Latitude \n";

    int i,j;
    for(i=0;i<ui->scaleTable->rowCount();i++)
    {
        for(j=0;j<2;j++)
        {
            out << ui->scaleTable->item(i,j)->text();
            if(j==0) out << ";";
        }
        out << "\n";
    }
    file.close();
}

/*
 * Sets the data in the table widget
 */
void resultForm::setTable()
{
    if(ui->sciBox->isChecked()) sci = 'g';
    else sci = 'f';

    int i,j;
    for(i=0;i<zoomLevels;i++)
    {
        for(j=0;j<COLS;j++)
        {
            if(j<4)
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(mapData[i][j],'f',0)));
            else
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(QString::number(mapData[i][j],sci,precision)));
        }
    }
}

/*
 * Computes the table data based on the current variable values
 */
void resultForm::compute()
{
    int i;

    float phi = boundingBox[0] - (boundingBox[0]-boundingBox[1])/2;

    precision = ui->precBox->value();

    for(i=0;i<zoomLevels;i++)
    {
        /* Set the zoom level */
        mapData[i][0]=i;

        /* Calculate the number of columns */
        mapData[i][2]=pow(2,i);

        /* Calculate the number of rows */
        if(ui->sampleButton->isChecked()){
            if(i>0) mapData[i][3]= mapData[i][2]/2;
            else mapData[i][3] = 1;
        }
        else
            mapData[i][3] = mapData[i][2];

        /* Calculate the number of tiles */
        mapData[i][1] = mapData[i][2] * mapData[i][3];

        /* Calculate Degrees longitude per tile */
        mapData[i][4] = mapWidth / mapData[i][2];

        /* Calculate Degrees latitude per tile */
        mapData[i][5] = mapHeight / mapData[i][3];

        /* Calculate Degrees per pixel */
        mapData[i][6] = mapWidth / (pow(2,i) * tileWidth);

        /* Calculate Ground resolution at latitude phi */
        mapData[i][7] = cos(phi*(M_PI/180))*2*M_PI*
                (EARTH_CIRCUM/(512*pow(2,i)));

        /* Calculate Map Scale */
        mapData[i][8] = (dpi/IN_TO_M)*mapData[i][7];
    }

    setTable();
}

/*
 * Takes in a desired map scale and calculates the central
 * latitude necessary to achieve the scale. Adds the result
 * to the scale table.
 */
void resultForm::addLatRow(int scale)
{
    int i,index=0;
    float lat=0,diff=fabs(mapData[index][8]- scale);
    for(i=0;i<zoomLevels;i++)
    {
        if(fabs(mapData[i][8]-scale)<diff)
        {
            diff=fabs(mapData[i][8]-scale);
            index = i;
        }
    }
    float res = scale*IN_TO_M/dpi;
    lat = (512*pow(2,index)*res)/(2*M_PI*EARTH_CIRCUM);

    if(lat > 1)
    {
        lat=90-((-lat+M_PI)*(180/M_PI));
    }
    else
    {
        lat = acos(lat)*(180/M_PI);
    }

    int row = ui->scaleTable->rowCount();
    ui->scaleTable->insertRow(row);
    ui->scaleTable->setItem(row,0,new QTableWidgetItem(QString::number(scale)));
    ui->scaleTable->setItem(row,1,new QTableWidgetItem(QString::number(lat)));
}

void resultForm::addRow(){
    int scale = ui->scaleText->text().toInt();
    if(scale>0) addLatRow(scale);
}

/* Default destructor */
resultForm::~resultForm()
{
    delete ui;
}
