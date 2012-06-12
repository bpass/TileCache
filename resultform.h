////////////////////////////////////////////
/* Created by Brian Passuello             */
/* Property of USGS                       */
/* Last edited 06/12/12                   */
////////////////////////////////////////////

#ifndef RESULTFORM_H
#define RESULTFORM_H

#include <QWidget>
#include "utility.h"
#include "mainwindow.h"

namespace Ui {
class resultForm;
}

class resultForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit resultForm(QWidget *parent = 0);
    void initialize(float[4],int,int,int,float,float);
    ~resultForm();
    
private slots:
    void save();
    void setTable();
    void compute();
private:
    Ui::resultForm *ui;
};

#endif // RESULTFORM_H
