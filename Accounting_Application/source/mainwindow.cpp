#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CompteManager.h"
#include "CompteAbstrait.h"
#include "declaration.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->formAjouterCompteVirtuel->hide();
    ui->formTransactionMainWidget->hide();
    ui->operationsCompteSelectionne->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableTransactions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    fenetreSoldeInitial = new FenetreSoldeInitial(this);
    fenetreCloture = new FenetreCloture(this);
    fenetreReleve = new FenetreReleve(this);

    QObject::connect(ui->arborescenceComptes, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(traitementCompteSelectionne(QTreeWidgetItem*)));
    QObject::connect(ui->type, SIGNAL(currentIndexChanged(QString)), this, SLOT(changerFormSoldeInitial(QString)));
    QObject::connect(ui->boutonValiderCreationCompte, SIGNAL(clicked()), this, SLOT(creerNouveauCompte()));
    QObject::connect(ui->boutonValiderCreationTransaction, SIGNAL(clicked()), this, SLOT(creerNouvelleTransaction()));
    QObject::connect(ui->boutonAnnulerCreationTransaction, SIGNAL(clicked()), this, SLOT(resetFormOperations()));
    QObject::connect(ui->boutonAjouterOp, SIGNAL(clicked()), this, SLOT(ajouterFormOperation()));
    QObject::connect(ui->boutonAnnulerModif,SIGNAL(clicked()), this, SLOT(resetTransactions()));
    QObject::connect(ui->boutonValiderModif,SIGNAL(clicked()), this, SLOT(modifierTransactions()));
    QObject::connect(ui->tableTransactions,SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(setEditingTransactionsTrue()));
    QObject::connect(ui->actionCreer_un_compte, SIGNAL(triggered()), this, SLOT(resetFormOperations()));
    try {
        chargerContexte();
    } catch(TresorerieException& e) { QMessageBox::critical(this, "Erreur de récupération du contexte", e.getInfo()); }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept(); //on accepte par défaut la fermeture de l'appli

    int reponse = QMessageBox::No;
    if (currentProject != "" && !hasSaved)
        reponse = QMessageBox::warning(this, "Sauvegarder", "Certaines modifications n'ont pas été sauvegardées. Voulez vous sauvegarder avant de quitter ?",
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reponse == QMessageBox::Cancel) {
        event->ignore(); //si l'utilisateur décide d'annuler on ne ferme pas
        return;
    }

    if (reponse == QMessageBox::Yes)
        on_actionSauvegarder_triggered();

    if (currentProject != "") //pas besoin de sauvegarder le contexte si on est pas dans un projet
        sauvegarderContexte();
}

void MainWindow::activerActions()
{
    ui->actionCloture->setEnabled(true);
    ui->actionRapprocher->setEnabled(true);
    ui->actionSauvegarder->setEnabled(true);
    ui->actionCreer_un_compte->setEnabled(true);
    ui->actionSupprimer_un_compte->setEnabled(true);
    ui->actionCreerTransaction->setEnabled(true);
    ui->actionGenererReleve->setEnabled(true);
    ui->actionGenererBilan->setEnabled(true);
    ui->actionGenererResultat->setEnabled(true);
}

void MainWindow::setHasSaved(bool value)
{
    hasSaved = value;
    if (value == false)
        setWindowTitle(QString("Gestion de Trésorerie") + " - " + currentProject + "*");
    else
        setWindowTitle(QString("Gestion de Trésorerie") + " - " + currentProject);
}
