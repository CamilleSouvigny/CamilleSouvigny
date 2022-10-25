#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CompteManager.h"
#include "CompteAbstrait.h"
#include "declaration.h"
#include <QMessageBox>
#include <QLocale>

void MainWindow::traitementCompteSelectionne(QTreeWidgetItem* item) {
    if (item != nullptr)
    {
        CompteManager& cm = CompteManager::getManager();
        AbstractCompte* compteCourant = cm.getCompte(item->text(COLONNE_ID).toUInt());
        ui->labelNomCompteSelectionne->setText(item->text(COLONNE_NOM));
        affichageTransactions(compteCourant);
        affichageOperations(compteCourant);
        affichageHistoriqueRapprochement(compteCourant);
        /* ========== Traitement du formulaire creation compte ============ */
        ui->valeurParent->setText(item->text(COLONNE_NOM));
        if (compteCourant->getId() != 0) { //si la sélection n'est pas sur le compte racine, pas besoin de demander le type du nouveau compte à l'utilisateur
            ui->type->setCurrentIndex(compteCourant->getType());
            ui->type->setEnabled(false);
        }
        else
            ui->type->setEnabled(true);
    }
}

/* ================================================================================================================ *
 * ===================================== Gestion de la création d'un compte ======================================= *
 * ================================================================================================================ */

void MainWindow::creerNouveauCompte()
{
    CompteManager& cm = CompteManager::getManager();

    QTreeWidgetItem* parent = ui->arborescenceComptes->currentItem();
    const unsigned int& idCompteParent = parent->text(1).toUInt(); // on récupère l'ID du parent, le parent étant le compte actuellement sélectionné par l'utilisateur

    if (isFormCreationCompteValide(idCompteParent))
    {
        const TypeCompte& typeNouveauCompte = convertStringToEnum(ui->type->currentText());

        const double& soldeInitial = ui->valeurSoldeInitial->value();

        try {
            const QString& nom = ui->nom->text();
            if (nom == "Resultat" || nom == "Deficit" || nom == "Excedent")
                throw TresorerieException("Impossible d'utiliser le nom de compte \"" + nom + " \" : Ce nom est reserve pour la cloture");

            if (soldeInitial != 0) {
                Compte* capitauxPropre = getCapitauxPropre();
                cm.creerCompte(ui->nom->text(), typeNouveauCompte, idCompteParent, capitauxPropre, soldeInitial);
            }
            else cm.creerCompte(ui->nom->text(), typeNouveauCompte, ui->checkBoxVirtuel->isChecked(), idCompteParent);
            afficherTousLesComptes();
            resizeTreeWidgetColumns();
            setHasSaved(false);
        }
        catch (const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
    }
}

Compte* MainWindow::getCapitauxPropre()
{
    fenetreSoldeInitial->rafraichirListeCompte();
    fenetreSoldeInitial->exec();
    if (fenetreSoldeInitial->result() == QDialog::Rejected)
        throw TresorerieException("Abandon de la création du compte");

    return fenetreSoldeInitial->getCompteChoisi();
}

bool MainWindow::isFormCreationCompteValide(unsigned int idCompteParent)
{
    AbstractCompte* compteParent = CompteManager::getManager().getCompte(idCompteParent);

    if (ui->nom->text().isEmpty()) {
        QMessageBox::critical(this, "Erreur", "Veuillez rentrer un nom");
        return false;
    }
    else if (ui->valeurParent->text().isEmpty()) {
        QMessageBox::critical(this, "Erreur", "Veuillez selectionner un compte parent");
        return false;
    }
    else if (!compteParent->isVirtuel()) {
        QMessageBox::critical(this, "Erreur", "Compte non virtuel : impossible d'ajouter un sous compte");
        return false;
    }
    else if (ui->valeurSoldeInitial->value() != 0 && ui->checkBoxVirtuel->isChecked()) {
        QMessageBox::critical(this, "Erreur", "Un compte virtuel ne peut pas avoir de compte initial");
        return false;
    }
    return true;
}

void MainWindow::changerFormSoldeInitial(QString type) // empeche l'utilisateur de mettre un solde initial sur un compte différent de passif ou actif
{
    if (type == "Actif" || type == "Passif")
        ui->valeurSoldeInitial->setEnabled(true);
    else {
        ui->valeurSoldeInitial->setEnabled(false);
        ui->valeurSoldeInitial->setValue(0);
    }
}

/* ================================================================================================================ *
 * ===================================== Gestion de la suppression d'un compte ==================================== *
 * ================================================================================================================ */

void MainWindow::on_actionSupprimer_un_compte_triggered()
{
    try {
        QTreeWidgetItem* itemCourant = ui->arborescenceComptes->currentItem();
        if (itemCourant != nullptr) {
            if (itemCourant == ui->arborescenceComptes->topLevelItem(0))
                throw TresorerieException("Impossible de supprimer le compte racine");

            const unsigned int& idCompte = itemCourant->text(1).toUInt();
            AbstractCompte* c = CompteManager::getManager().getCompte(idCompte);
            if (c->hasTransactionsRapprochees())
                throw TresorerieException("Impossible de supprimer le compte car il a été rapproché ou il contient des transactions rapprochées");

            int reponse = QMessageBox::warning(this, "Attention !", "Cette action entraînera la suppression des sous-comptes ainsi que la totalité des transactions lié à ce compte. Etes-vous sûr de vouloir continuer ?", QMessageBox::Yes | QMessageBox::No);
            if (reponse == QMessageBox::Yes) {
                CompteManager::getManager().supprimerCompte(c);
                afficherTousLesComptes();
                setHasSaved(false);
            }

        }
    } catch(const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }

}

/* ================================================================================================================ *
 * ============================ Gestion de l'affichage des comptes dans l'arborescence ============================ *
 * ================================================================================================================ */

void MainWindow::afficherTousLesComptes()
{
    QTreeWidgetItem* itemCourant = ui->arborescenceComptes->currentItem();
    QString idCompteCourant = "0";
    if (itemCourant != nullptr)
        idCompteCourant = itemCourant->text(COLONNE_ID);

    ui->arborescenceComptes->clear();

    const CompteVirtuel* compteRacine = CompteManager::getManager().getRacine();
    QVector<AbstractCompte*> sousComptes = compteRacine->getSousComptes();

    QTreeWidgetItem* racine = new QTreeWidgetItem({compteRacine->getNom()});
    racine->setIcon(0,QIcon("../icons/compte_virtuel.png"));
    ui->arborescenceComptes->addTopLevelItem(racine);

    afficherSousComptesSurArbre(sousComptes, racine);
    resizeTreeWidgetColumns();

    QList<QTreeWidgetItem*> list = ui->arborescenceComptes->findItems(idCompteCourant, Qt::MatchRecursive, COLONNE_ID);
    if (!list.isEmpty())
        ui->arborescenceComptes->setCurrentItem(list.first()); //on remet l'item courant
    else
        ui->arborescenceComptes->setCurrentItem(racine);

}

QTreeWidgetItem* MainWindow::afficherCompteSurArbre(AbstractCompte* compte, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* tmp = new QTreeWidgetItem({compte->getNom(), QString::number(compte->getId()), QLocale(QLocale().language(),QLocale().country()).toCurrencyString(compte->getSolde()).remove(QRegExp("[()]"))} );
    if(compte->isVirtuel())
        tmp->setIcon(0,QIcon("../icons/compte_virtuel.png"));
    else
        tmp->setIcon(0,QIcon("../icons/compte_concret.png"));
    parent->addChild(tmp);
    parent->setExpanded(true);
    return tmp;
}

void MainWindow::afficherSousComptesSurArbre(QVector<AbstractCompte*> comptes, QTreeWidgetItem *parent)
{
    for(int i=0 ; i<comptes.size() ; i++)
    {
        QTreeWidgetItem* itemCompteCourant = afficherCompteSurArbre(comptes[i], parent);
        if (comptes[i]->isVirtuel())
        {
            afficherSousComptesSurArbre(static_cast<CompteVirtuel*>(comptes[i])->getSousComptes(), itemCompteCourant);
        }
    }
}

void MainWindow::resizeTreeWidgetColumns() const
{
    for(int i=0 ; i<ui->arborescenceComptes->columnCount() ; i++)
        ui->arborescenceComptes->resizeColumnToContents(i);
}

/* ================================================================================================================ *
 * =================================== Gestion de la cloture et du rapprochement ================================== *
 * ================================================================================================================ */

void MainWindow::on_actionCloture_triggered()
{
    try {
        fenetreCloture->exec();
        if (fenetreCloture->result() == QDialog::Rejected)
            QMessageBox::critical(this, "Erreur", "Abandon de la cloture");
        afficherTousLesComptes();
        setHasSaved(false);
    } catch (TresorerieException& t) {std::cout<<t.getInfo().toStdString();}
}

void MainWindow::on_actionRapprocher_triggered()
{
    try {
        QTreeWidgetItem* item = ui->arborescenceComptes->currentItem();
        if (item == nullptr)
            throw TresorerieException("Veuillez sélectionner un compte à rapprocher");

        unsigned int idCompte = item->text(COLONNE_ID).toUInt();

        CompteManager& cm = CompteManager::getManager();

        AbstractCompte* compteARapprocher = cm.getCompte(idCompte);

        if (compteARapprocher->getId() == 0)
            cout<<"yolo";



        double soldeActuel = compteARapprocher->getSolde();
        double dernierSolde = compteARapprocher->getHistorique().getDernierSolde();

        QString text = "Solde au dernier rapprochement : " + QString::number(dernierSolde) + "\n"
                        + "Solde actuel : " + QString::number(soldeActuel) + "\n"
                        + "Etes-vous sûr de vouloir rapprocher ce compte ?";

        int reponse = QMessageBox::question(this, "Rapprochement", text);

        if(reponse == QMessageBox::Yes){
            compteARapprocher->rapprocher();
            affichageTransactions(compteARapprocher);
            affichageHistoriqueRapprochement(compteARapprocher);
            setHasSaved(false);
        }

    }catch(const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
}

void MainWindow::affichageHistoriqueRapprochement(AbstractCompte* compteCourant)
{
    if (compteCourant->getId() != 0)
    {
        QString markdownHistorique("## Historique des rapprochements du compte " + compteCourant->getNom() + "\n<br/><br/>\n");

        RapprochementCareTaker& historique = compteCourant->getHistorique();
        auto it = historique.cbegin();

        markdownHistorique += "### Solde initial \n";
        markdownHistorique += "- Date : " + (*it)->getDate().toString() + "\n";
        markdownHistorique += "- Solde : " + QString::number((*it)->getSolde()) + "\n";

        int i=1;
        for (++it ; it!= historique.cend(); ++it)
        {
            markdownHistorique += "### Rapprochement " + QString::number(i) + "\n";
            markdownHistorique += "- Date : " + (*it)->getDate().toString() + "\n";
            markdownHistorique += "- Solde : " + QString::number((*it)->getSolde()) + "\n";
            i++;
        }

        ui->textHistorique->setMarkdown(markdownHistorique);
    }
}
