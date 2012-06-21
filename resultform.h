////////////////////////////////////////////
/* Created by Brian Passuello             */
/* Property of USGS                       */
/* Last edited 06/21/12                   */
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
    void addLatRow(int scale);
    ~resultForm();
    
private slots:
    void save();
    void saveScale();
    void setTable();
    void compute();
    void addRow();
private:
    Ui::resultForm *ui;
};

#endif // RESULTFORM_H
