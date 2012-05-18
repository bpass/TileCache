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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    int compute();
    void test10();
    void testBrazil();
    void testWorld();
    void testConus();
    void testAK();
    void reset();
    
private:
    Ui::MainWindow *ui;
    int compute(float[4],int,int,int,int,float,float);
};

#endif // MAINWINDOW_H
