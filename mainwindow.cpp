////////////////////////////////////////////
/* Created by Brian Passuello             */
/* Property of USGS                       */
/* Last edited 06/12/12                   */
////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "resultform.h"
#include <QMessageBox>
#include <math.h>
#include <iostream>

using namespace std;

/*
 * Constructor
 */
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

/*
 * Resets the text box values to their default values
 */
void MainWindow::reset(){
    ui->highLatBox->clear();
    ui->lowLatBox->clear();
    ui->highLongBox->clear();
    ui->lowLongBox->clear();
    ui->dpiBox->setText("96");
    ui->pixBox->setText("512");
    ui->zoomBox->setText("20");
}

/*
 * Computes minimum bounding rectangle based on given dimensions
 * and passes parameters to resultform.initialize()
 */
void MainWindow::compute()
{
    int zoom_levels = ui->zoomBox->text().toInt();
    int dpi = ui->dpiBox->text().toInt();
    int pix_width = ui->pixBox->text().toInt();
    float maxLong = ui->highLongBox->text().toFloat();
    float minLong = ui->lowLongBox->text().toFloat();
    float maxLat = ui->highLatBox->text().toFloat();
    float minLat = ui->lowLatBox->text().toFloat();

    /* Check if all values have been entered */
    if(ui->highLatBox->text().isEmpty() ||
            ui->highLongBox->text().isEmpty() ||
            ui->lowLatBox->text().isEmpty() ||
            ui->lowLongBox->text().isEmpty())
    {
        QMessageBox msg;
        msg.setText("Error: All bounding values not set.");
        msg.exec();
        return;
    }

    /* Check if values are reasonable */
    if(maxLong > 180 || minLong < -180 || maxLat > 90 || minLat < -90)
    {
        QMessageBox msg;
        msg.setText("Error: Please check longitudes are between -180 and 180 and latitudes are between -90 and 90");
        msg.exec();
        return;
    }

    /* Check if the person is showing more than the whole world */
    if(fabsf(maxLong-minLong)>360 || fabsf(maxLat-minLat) > 180)
    {
        QMessageBox msg;
        msg.setText("Error: Longitudes must be within 360 of each other and latitudes must be within 180.");
        msg.exec();
        return;
    }

    /* Check to make sure zoom is reasonable */
    if(zoom_levels < 1 || zoom_levels > MAX_ZOOM)
    {
        zoom_levels=DEFAULT_ZOOM;
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

    float deltaLat = 2 * (maxLat - minLat);
    float deltaLong;

    /* Cases for if longitudes cross international date line */
    if(maxLong>minLong) deltaLong = maxLong - minLong;
    else deltaLong = 360-(minLong-maxLong);

    /* MBR Code to make long to lat ratio 2:1 */
    if(deltaLat < deltaLong)
    {
        maxLat = minLat + (deltaLong / 2);
        if(maxLat>90)
        {
            minLat-=(fmod(maxLat,90));
            maxLat=90;
        }
    }
    else if(deltaLat > deltaLong)
    {
        maxLong = minLong + deltaLat;
        if(maxLong>180)
        {
            minLong-=(fmod(maxLong,90));
            maxLong=180;
        }
    }

    zoom_levels++;
    float bb[4] = {maxLat,minLat,maxLong,minLong};
    resultForm* results = new resultForm();
    results->initialize(bb,dpi,pix_width,zoom_levels,deltaLong,deltaLat);
    results->show();
}

/*
 * Default destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
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
