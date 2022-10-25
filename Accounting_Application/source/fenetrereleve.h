#ifndef FENETRERELEVE_H
#define FENETRERELEVE_H

#include <QDialog>

namespace Ui {
class FenetreReleve;
}

class FenetreReleve : public QDialog
{
    Q_OBJECT

public:
    explicit FenetreReleve(QWidget *parent = nullptr);
    ~FenetreReleve();
private slots:
    void genererReleve();

private:
    Ui::FenetreReleve *ui;
};

#endif // FENETRERELEVE_H
