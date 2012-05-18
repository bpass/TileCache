#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resultform.h"
#include <QMessageBox>
#include <math.h>
#include <iostream>

#define COLS 9
#define IN_TO_M 0.0254
#define EARTH_CIRCUM 6378137

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->resultsButton,SIGNAL(clicked()),this,SLOT(compute()));
    connect(ui->test10,SIGNAL(triggered()),this,SLOT(test10()));
    connect(ui->testBrazil,SIGNAL(triggered()),this,SLOT(testBrazil()));
    connect(ui->testWorld,SIGNAL(triggered()),this,SLOT(testWorld()));
    connect(ui->testConus,SIGNAL(triggered()),this,SLOT(testConus()));
    connect(ui->testAK,SIGNAL(triggered()),this,SLOT(testAK()));
    connect(ui->resetButton,SIGNAL(triggered()),this,SLOT(reset()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::reset(){
    ui->highLatBox->clear();
    ui->lowLatBox->clear();
    ui->highLongBox->clear();
    ui->lowLongBox->clear();
    ui->dpiBox->setText("96");
    ui->pixBox->setText("512");
    ui->zoomBox->setText("20");
}

int MainWindow::compute()
{
    int phi=0; //Latitude of map center
    float width,height;

    int zoom_levels = ui->zoomBox->text().toInt();
    int dpi = ui->dpiBox->text().toInt();
    int pix_width = ui->pixBox->text().toInt();
    float maxLong = ui->highLongBox->text().toFloat();
    float minLong = ui->lowLongBox->text().toFloat();
    float maxLat = ui->highLatBox->text().toFloat();
    float minLat = ui->lowLatBox->text().toFloat();
    float deltaLong = maxLong - minLong;
    float deltaLat = 2 * (maxLat - minLat);
    float originalBB[4] = {minLong,maxLong,minLat,maxLat};

    /* Check if all values have been entered */
    if(ui->highLatBox->text().isEmpty() ||
            ui->highLongBox->text().isEmpty() ||
            ui->lowLatBox->text().isEmpty() ||
            ui->lowLongBox->text().isEmpty())
    {
        QMessageBox msg;
        msg.setText("Error: All bounding values not set.");
        msg.exec();
        return 0;
    }

    /* Check if values are reasonable */
    if(maxLong > 180 || minLong < -180 || maxLat > 90 || minLat < -90)
    {
        QMessageBox msg;
        msg.setText("Error: Please check longitudes are between -180 and 180 and latitudes are between -90 and 90");
        msg.exec();
        return 0;
    }

    /* Check if mins are less than maxes */
    if(minLong >= maxLong || minLat >= maxLat)
    {
        QMessageBox msg;
        msg.setText("Error: Make sure minimum longitude and latitude are strictly less than their respective maximums");
        msg.exec();
        return 0;
    }

    /* Check to make sure zoom is reasonable */
    if(zoom_levels < 1 || zoom_levels > 30)
    {
        zoom_levels=20;
        QMessageBox msg;
        msg.setText("Error: Make sure the level of zoom is between 1 and 30. Reverting to default of 20.");
        msg.exec();
    }

    /* Check that dpi is reasonable */
    if(dpi < 1 || dpi > 5000) /// TODO: Find what max dpi should be
    {
        QMessageBox msg;
        msg.setText("Error: Make sure DPI is between 1 and 5000");
        msg.exec();
        dpi = 96;
    }

    /* Check that pixels per tile is reasonable */
    if(pix_width < 1 || pix_width > 5000)
    {
        QMessageBox msg;
        msg.setText("Error: Make sure pixels per tile is between 1 and 5000");
        msg.exec();
        pix_width = 512;
    }

    /* MBR Code to make long to lat ratio 2:1 */
    if(deltaLat < deltaLong) maxLat = minLat + (deltaLong / 2);
    else if(deltaLat > deltaLong) maxLong = minLong + deltaLat;

    /*Set phi to center of map latitude*/
    phi = maxLat - (maxLat-minLat)/2;

    /* Calculate width and height of the map section */
    height= maxLat - minLat;
    width= maxLong - minLong;

    zoom_levels++;
    float bb[4] = {maxLat,minLat,maxLong,minLong};
    return compute(bb,pix_width,dpi,phi,zoom_levels,width,height);
}

int MainWindow::compute(float bb[4],int pix_width, int dpi, int phi, int zoom_levels, float width, float height)
{
    int i;

    float arr[zoom_levels][COLS];

    for(i=0;i<zoom_levels;i++)
    {
        /*Zoom Level*/
        arr[i][0]=i;

        /*Num Cols*/
        arr[i][2]=pow(2,i);

        /*Num Rows*/
        if(i>0) arr[i][3]=arr[i][2]/2;
        else arr[i][3] = 1;

        /*Num Tiles*/
        arr[i][1] = arr[i][2] * arr[i][3];

        /*Degrees Long per tile */
        arr[i][4] = width / arr[i][2];

        /*Degrees lat per tile */
        arr[i][5] = height / arr[i][3];

        /*Degrees per pixel*/
        arr[i][6] = width / (pow(2,i) * pix_width);

        /*Ground resolution at latitude phi*/
        arr[i][7] = cos(phi*(M_PI/180))*2*M_PI*
                (EARTH_CIRCUM/(512*pow(2,i)));

        /* Map Scale */
        arr[i][8] = (dpi/IN_TO_M)*arr[i][7];
    }

    resultForm* results = new resultForm();
    results->initialize(bb, dpi, pix_width, zoom_levels,arr);
    results->show();

    return 1;
}




/**************** TEST CASES ******************/



/* Run a test with zoomlevel=10, lat = 45, long = 80 */
void MainWindow::test10(){
    ui->lowLatBox->setText("-20");
    ui->highLatBox->setText("25");
    ui->lowLongBox->setText("20");
    ui->highLongBox->setText("100");
    ui->zoomBox->setText("10");
    compute();
}

void MainWindow::testBrazil(){
    ui->lowLatBox->setText("-34");
    ui->highLatBox->setText("5");
    ui->lowLongBox->setText("-74");
    ui->highLongBox->setText("-34");
    ui->zoomBox->setText("15");
    compute();
}

void MainWindow::testWorld(){
    ui->lowLatBox->setText("-90");
    ui->highLatBox->setText("90");
    ui->lowLongBox->setText("-180");
    ui->highLongBox->setText("180");
    ui->zoomBox->setText("20");
    compute();
}

void MainWindow::testConus(){
    ui->lowLatBox->setText("17");
    ui->highLatBox->setText("73");
    ui->lowLongBox->setText("-60");
    ui->highLongBox->setText("170");
    ui->zoomBox->setText("20");
    compute();
}

void MainWindow::testAK(){
    ui->lowLatBox->setText("50");
    ui->highLatBox->setText("72");
    ui->lowLongBox->setText("-128");
    ui->highLongBox->setText("170");
    ui->zoomBox->setText("20");
    compute();
}
