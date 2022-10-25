#include "fenetresoldeinitial.h"
#include "ui_fenetresoldeinitial.h"

FenetreSoldeInitial::FenetreSoldeInitial(QWidget *parent) : QDialog(parent), ui(new Ui::FenetreSoldeInitial)
{
    ui->setupUi(this);
}

void FenetreSoldeInitial::rafraichirListeCompte()
{
    ajouterComptesCapitauxPropre();
    ajouterComptesParents();
}

void FenetreSoldeInitial::ajouterComptesCapitauxPropre()
{
    ui->listeComptesCapitauxPropre->clear();
    for (auto it=cm.getIterator() ; !it.isDone() ; it.next())
    {
        AbstractCompte* currentCompte = it.currentItem();
        if (!currentCompte->isVirtuel() && currentCompte->getType() == passif)
        {
            ui->listeComptesCapitauxPropre->addItem(currentCompte->getNom() + " (Parent: " + currentCompte->getParent().getNom() + ")", QVariant(currentCompte->getId()));
        }
    }
}

void FenetreSoldeInitial::ajouterComptesParents()
{
    ui->listeParentPassif->clear();
    auto it=cm.getIterator();
    ui->listeParentPassif->addItem(it.currentItem()->getNom() + " (Racine)", QVariant(it.currentItem()->getId()));

    for (it.next() ; !it.isDone() ; it.next())
    {
        AbstractCompte* currentCompte = it.currentItem();
        if (currentCompte->isVirtuel() && currentCompte->getType() == passif)
        {
            ui->listeParentPassif->addItem(currentCompte->getNom() + " (Parent: " + currentCompte->getParent().getNom() + ")", QVariant(currentCompte->getId()));
        }
    }
}

void FenetreSoldeInitial::on_boutonCreerCompte_clicked()
{
    QString nomCompte = ui->valeurNomCompte->text();
    if (nomCompte == "")
        QMessageBox::critical(this, "Erreur", "Veuillez entrer un nom de compte valide");
    else
    {
        cm.creerCompte(nomCompte, passif, 0, ui->listeParentPassif->currentData().toUInt());
        ui->listeComptesCapitauxPropre->clear();
        ajouterComptesCapitauxPropre();
    }
}

Compte* FenetreSoldeInitial::getCompteChoisi()
{
    unsigned int idCompteChoisi = ui->listeComptesCapitauxPropre->currentData().toUInt();
    return static_cast<Compte*>(cm.getCompte(idCompteChoisi));
}

FenetreSoldeInitial::~FenetreSoldeInitial()
{
    delete ui;
}
