#ifndef FENETRECLOTURE_H
#define FENETRECLOTURE_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class FenetreCloture;
}

class FenetreCloture : public QDialog
{
    Q_OBJECT

public:
    explicit FenetreCloture(QWidget *parent = 0);
    ~FenetreCloture();
private slots:
    void genererCloture();

private:
    Ui::FenetreCloture *ui;

};

#endif // FENETRECLOTURE_H
