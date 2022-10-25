#include "fenetrecloture.h"
#include "ui_fenetrecloture.h"
#include "transactionmanager.h"
#include "CompteManager.h"

FenetreCloture::FenetreCloture(QWidget *parent) : QDialog(parent), ui(new Ui::FenetreCloture)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(genererCloture()));
}

FenetreCloture::~FenetreCloture()
{
    delete ui;
}

void FenetreCloture::genererCloture(){
    CompteManager& cm = CompteManager::getManager();

    try {
        cm.realiserCloture(ui->refCloture->text(), ui->nomCloture->text(), ui->refConstatation->text(), ui->nomConstatation->text());
    }
    catch (const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
}

