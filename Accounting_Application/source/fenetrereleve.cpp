#include "fenetrereleve.h"
#include "ui_fenetrereleve.h"
#include "CompteManager.h"
#include <QMessageBox>

FenetreReleve::FenetreReleve(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FenetreReleve)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate().addYears(-1));
    ui->dateEdit_2->setDate(QDate::currentDate());
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(genererReleve()));
}

FenetreReleve::~FenetreReleve()
{
    delete ui;
}

void FenetreReleve::genererReleve(){
    CompteManager& cm = CompteManager::getManager();
    try {

        if(ui->dateEdit->date() > ui->dateEdit_2->date())
            throw TresorerieException("La date de fin ne peut pas être antérieure à la date de début");
        cm.genererReleve(ui->dateEdit->date(),ui->dateEdit_2->date());
    }
    catch (const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
}
