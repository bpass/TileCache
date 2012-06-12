////////////////////////////////////////////
/* Created by Brian Passuello             */
/* Property of USGS                       */
/* Last edited 06/12/12                   */
////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "utility.h"
#include "resultform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void compute();
    void test10();
    void testBrazil();
    void testWorld();
    void testConus();
    void testAK();
    void reset();
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
