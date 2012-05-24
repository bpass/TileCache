#ifndef RESULTFORM_H
#define RESULTFORM_H

#include <QWidget>

namespace Ui {
class resultForm;
}

class resultForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit resultForm(QWidget *parent = 0);
    void initialize(float[4],int,int,int,float[][9]);
    ~resultForm();
    
private slots:
    void save();
private:
    Ui::resultForm *ui;
};

#endif // RESULTFORM_H
