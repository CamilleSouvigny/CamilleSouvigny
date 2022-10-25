#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>
#include <QObject>
#include <QMainWindow>
#include <QFormLayout>
#include <QComboBox>
#include "CompteAbstrait.h"
#include "CompteManager.h"
#include "filemanager.h"
#include "fenetresoldeinitial.h"
#include "fenetrecloture.h"
#include "fenetrereleve.h"

#define COLONNE_NOM 0
#define COLONNE_ID 1
#define COLONNE_SOLDE 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /* ============ Gestion des comptes ============== */
    void traitementCompteSelectionne(QTreeWidgetItem* item);
    void creerNouveauCompte();
    void on_actionSupprimer_un_compte_triggered();
    void on_actionCloture_triggered();
    void on_actionRapprocher_triggered();
    void changerFormSoldeInitial(QString type);

    /* ============ Gestion des transactions ============== */
    /*! permet de créer une nouvelle transaction depuis l'interface*/
    void creerNouvelleTransaction();
    void filtrerTransactions();
    /*! getion du signal et de l'affichage lors de la création d'une transaction*/
    void on_actionCreerTransaction_triggered();
    /*! Slot retrait d'un mini formulaire d'operation dans le formulaire d'ajout de Transaction*/
    void on_boutonRetirerOp_clicked();
    /*! Slot d'ajout d'un mini formulaire d'operation dans le formulaire d'ajout de Transaction*/
    void ajouterFormOperation(const unsigned int& idComboBox = 0, const double& credit = 0, const double& debit = 0);
    void resetFormOperations();
    void resetTransactions();
    /*! permet la modification d'une transaction depuis l'interface*/
    void modifierTransactions();
    /*! la transaction a été éditée*/
    void setEditingTransactionsTrue();

    /* ============ Gestion des fichiers/projets ============== */
    void on_actionImporterFichier_triggered();
    void on_actionSauvegarder_triggered();
    void on_actionCreer_un_nouveau_projet_triggered();

    /* ============ Génération des documents ============== */
    /*! permet de liée la fonction générer relevé au bouton de l'interface destinée à cet usage*/
    void on_actionGenererReleve_triggered() {
        fenetreReleve->exec();
    }
    /*! permet de liée la fonction générer bilan de la classe compteManager au bouton de l'interface destiné à cet usage*/
    void on_actionGenererBilan_triggered() { CompteManager::getManager().genererBilan(); }
    /*! permet de liée la fonction générer Resultat de la classe compteManager au bouton de l'interface destiné à cet usage*/
    void on_actionGenererResultat_triggered() { CompteManager::getManager().genererResultat(); }


private:
    /* =========== Attributs ============== */
    Ui::MainWindow *ui;
    QVector<QFormLayout*> formOperations;
    FenetreSoldeInitial* fenetreSoldeInitial;
    FenetreCloture* fenetreCloture;
    FenetreReleve* fenetreReleve;
    QString currentProject = "";
    bool hasSaved = false;
    bool editingTransactions = false;

    /* ============ Gestion des comptes ============== */
    bool isFormCreationCompteValide(unsigned int idCompteParent);
    Compte* getCapitauxPropre();
    void resizeTreeWidgetColumns() const;
    void afficherTousLesComptes();
    QTreeWidgetItem* afficherCompteSurArbre(AbstractCompte* compte, QTreeWidgetItem* parent);
    void afficherSousComptesSurArbre(QVector<AbstractCompte*> comptes, QTreeWidgetItem* parent);
    void ajouterComptesConcret(QComboBox* comptes);

    /* =========== Gestion des affichages =========== */
    void affichageOperations(AbstractCompte* compteCourant);
    void affichageTransactions(AbstractCompte* compteCourant);
    void affichageHistoriqueRapprochement(AbstractCompte* compteCourant);
    void supprimerLayout(QLayout* layout);

    /* ========== Gestion de le reprise de contexte ========== */
    void sauvegarderContexte();
    void chargerContexte();
    void importerFichier(const QString& cheminFichier);

    void setHasSaved(bool value);
    void activerActions();
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
