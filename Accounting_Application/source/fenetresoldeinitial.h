#ifndef FENETRESELECTIONCAPITAUXPROPRE_H
#define FENETRESELECTIONCAPITAUXPROPRE_H

#include <QDialog>
#include <QMessageBox>
#include "CompteManager.h"

namespace Ui {
class FenetreSoldeInitial;
}

class FenetreSoldeInitial : public QDialog
{
    Q_OBJECT

public:
    explicit FenetreSoldeInitial(QWidget *parent = nullptr);
    ~FenetreSoldeInitial();
    Compte* getCompteChoisi();
    void rafraichirListeCompte();

private:
    CompteManager& cm = CompteManager::getManager();
    Ui::FenetreSoldeInitial *ui;

    void ajouterComptesCapitauxPropre();
    void ajouterComptesParents();

private slots:
    void on_boutonCreerCompte_clicked();
};

#endif // FENETRESELECTIONCAPITAUXPROPRE_H
