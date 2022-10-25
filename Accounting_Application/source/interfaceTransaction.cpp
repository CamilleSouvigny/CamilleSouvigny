#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CompteManager.h"
#include "CompteAbstrait.h"
#include "declaration.h"
#include <QMessageBox>


/* ================================================================================================================ *
 * ==================================== Gestion de la création d'une transaction ================================== *
 * ================================================================================================================ */

void MainWindow::on_actionCreerTransaction_triggered()
{
    ui->formAjouterCompteVirtuel->hide();
    ui->formTransactionMainWidget->show();
    //On remet le formulaire à zéro au cas où
    resetFormOperations();
    //On ajoute les 2 comptes obligatoires
    ajouterFormOperation();
    ajouterFormOperation();
}

void MainWindow::creerNouvelleTransaction()
{
    TransactionManager& tm = TransactionManager::getInstance();
    CompteManager& cm = CompteManager::getManager();

    try{
        // On récupère les informations liées à la transaction
        QString memo;
        if(ui->lineEditMemoTransaction->text().isEmpty())
            throw TresorerieException("Veuillez rentrer un nom de transaction");
        else
            memo = ui->lineEditMemoTransaction->text();

        QString ref = ui->lineEditRef->text();
        QRegularExpression regex("^SI[0-9]+$");
        if (regex.match(ref).hasMatch())
            throw TresorerieException("Impossible de créer une transaction avec la référence " + ref + " : Ce type de référence est réservé aux soldes initiaux");

        QDate date = ui->dateEditTransaction->date();

        // On récupère les informations des opérations
        QVector<Operation*> operations;
        for(int i=0; i < formOperations.size(); i++) {
            QFormLayout* opCourante = formOperations[i];
            QComboBox* cptSelect = static_cast<QComboBox*>(opCourante->itemAt(0,QFormLayout::FieldRole)->widget());
            Compte* cpt = static_cast<Compte*>(cm.getCompte(cptSelect->currentData().toInt()));
            QDoubleSpinBox* debit = static_cast<QDoubleSpinBox*>(opCourante->itemAt(1,QFormLayout::FieldRole)->widget());
            QDoubleSpinBox* credit = static_cast<QDoubleSpinBox*>(opCourante->itemAt(2,QFormLayout::FieldRole)->widget());
            operations.push_back(new Operation(cpt,debit->value(), credit->value()));
        }

        tm.creerTransaction(ref, date, memo, operations); //throw une exception si transaction non valide

        ui->formTransactionMainWidget->hide();
        ui->labelNomCompteSelectionne->show();
        ui->operationsCompteSelectionne->show();

        afficherTousLesComptes(); // pour afficher les comptes avec les soldes mis à jour
        setHasSaved(false);

        ui->dateEditTransaction->setDate(QDate::currentDate());
    }
    catch (TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
}

void MainWindow::ajouterFormOperation(const unsigned int& idComboBox, const double& credit, const double& debit) // Slot d'ajout d'un mini formulaire d'operation dans le formulaire d'ajout de Transaction
{
    formOperations.push_back(new QFormLayout());
    unsigned int nb = formOperations.size()-1;
    QString compte = "Compte "+ QString::number(nb);

    QComboBox* comptes = new QComboBox();
    ajouterComptesConcret(comptes);
    comptes->setCurrentIndex(idComboBox);

    QDoubleSpinBox* boxDebit = new QDoubleSpinBox();
    boxDebit->setRange(-1000000, 1000000);
    boxDebit->setDecimals(2);
    boxDebit->setValue(debit); //utile pour la reprise du contexte

    QDoubleSpinBox* boxCredit = new QDoubleSpinBox();
    boxCredit->setRange(-1000000, 1000000);
    boxCredit->setDecimals(2);
    boxCredit->setValue(credit);

    formOperations[nb]->addRow(new QLabel(compte), comptes);
    formOperations[nb]->addRow(new QLabel("Débit"), boxDebit);
    formOperations[nb]->addRow(new QLabel("Crédit"), boxCredit);
    ui->verticalLayoutOperation->insertLayout(nb, formOperations[nb]);
}

void MainWindow::ajouterComptesConcret(QComboBox* comptes){
    for (auto it=CompteManager::getManager().getIterator() ; !it.isDone() ; it.next())
    {
        AbstractCompte* c = it.currentItem();
        if(!c->isVirtuel())
            comptes->addItem(c->getNom()+ " (" + c->getParent().getNom() + ")", QVariant(c->getId()));
    }
}

void MainWindow::on_boutonRetirerOp_clicked() // Slot retrait d'un mini formulaire d'operation dans le formulaire d'ajout de Transaction
{
    if(formOperations.size() <= 2){
        QMessageBox::critical(this, "Erreur", "Le nombre de comptes impliqués dans une transaction ne peut pas être inférieur à 2");
    }
    else{
        supprimerLayout(formOperations.last());
        formOperations.pop_back();
    }
}

void MainWindow::supprimerLayout(QLayout* layout)
{
    QWidget* widget;
    QLayoutItem* item;
    while ((item = layout->takeAt(0))) {
        if ((widget = item->widget()) != 0) {
            widget->hide();
            delete widget;
        }
        else {
            delete item;
        }
    }
}

void MainWindow::resetFormOperations()
{
    while(!formOperations.empty()){
        supprimerLayout(formOperations.last());
        formOperations.pop_back();
    }
}

/* ================================================================================================================ *
 * ============================ Affichage des opérations et transactions d'un compte ============================== *
 * ================================================================================================================ */

void MainWindow::affichageOperations(AbstractCompte* compteCourant){
    ui->operationsCompteSelectionne->clearContents();
    ui->operationsCompteSelectionne->show();
    ui->labelNomCompteSelectionne->show();

    AbstractSolde* solde; //sert à recréer un historique des soldes du compte, pour afficher le solde après chaque opération
    if (compteCourant->getType() == actif || compteCourant->getType() == depense)
        solde = new SoldeEmploi();
    else
        solde = new SoldeRessource();


    if(compteCourant->isVirtuel()){
        ui->operationsCompteSelectionne->setRowCount(0);
        /*
         * Affichage pour les comptes virtuels ?
         */
    }
    else{
        try{
            Compte* compte = static_cast<Compte*>(compteCourant);
            const QVector<Operation*> operations = compte->getOperations();
            ui->operationsCompteSelectionne->setRowCount(operations.size());
            for(int i=0; i < operations.size();i++){ // Ajout des operations dans le TableWidget
                ui->operationsCompteSelectionne->setItem(i,0, new QTableWidgetItem(operations[i]->getTransaction()->getDate().toString("dd/MM/yyyy")));
                ui->operationsCompteSelectionne->setItem(i,1, new QTableWidgetItem(operations[i]->getTransaction()->getReference()));
                ui->operationsCompteSelectionne->setItem(i,2, new QTableWidgetItem(operations[i]->getTransaction()->getMemo()));
                ui->operationsCompteSelectionne->setItem(i,3, new QTableWidgetItem(QLocale(QLocale().language(),QLocale().country()).toCurrencyString(operations[i]->getDebit()).remove(QRegExp("[()]"))));
                ui->operationsCompteSelectionne->setItem(i,4, new QTableWidgetItem(QLocale(QLocale().language(),QLocale().country()).toCurrencyString(operations[i]->getCredit()).remove(QRegExp("[()]"))));
                solde->crediter(operations[i]->getCredit());
                solde->debiter(operations[i]->getDebit());
                ui->operationsCompteSelectionne->setItem(i,5, new QTableWidgetItem(QLocale(QLocale().language(),QLocale().country()).toCurrencyString(solde->getMontant()).remove(QRegExp("[()]"))));
                for (int j=0 ; j<=5 ; j++)
                {
                    QTableWidgetItem* tmp = ui->operationsCompteSelectionne->item(i,j);
                    tmp->setFlags(tmp->flags() & ~Qt::ItemIsEditable);
                }
            }
        }catch(TresorerieException* e) { QMessageBox::critical(this, "Erreur", e->getInfo()); }
    }
    delete solde;
}

void MainWindow::filtrerTransactions(){ //inutile ??

    ui->tableTransactions->clearContents();
    ui->tableTransactions->setRowCount(0);
    TransactionManager& tm = TransactionManager::getInstance();
    QTableWidget* tab = ui->tableTransactions;

    /* ======================== Affichage sans filtre ============================= */

        for(int i=0; i < tm.getTransactions().size(); i++){
            int transactionRow = tab->rowCount();
            tab->setRowCount(tab->rowCount()+1);
            tab->setItem(tab->rowCount()-1,0,new QTableWidgetItem(tm.getTransactions()[i]->getDate().toString("dd/MM/yyyy")));
            tab->setItem(tab->rowCount()-1,1,new QTableWidgetItem(tm.getTransactions()[i]->getReference()));
            tab->setItem(tab->rowCount()-1,2,new QTableWidgetItem(tm.getTransactions()[i]->getMemo()));
            //Ajout de case vide pour la coloration
            tab->setItem(tab->rowCount()-1,3,new QTableWidgetItem(""));
            tab->setItem(tab->rowCount()-1,4,new QTableWidgetItem(""));
            tab->setItem(tab->rowCount()-1,5,new QTableWidgetItem(""));

            QVector<Operation*> ops = tm.getTransactions()[i]->getOperations();
            for(int j=0; j < ops.size();j++){
                tab->setRowCount(tab->rowCount()+1);
                tab->setItem(tab->rowCount()-1,0,new QTableWidgetItem(""));
                tab->setItem(tab->rowCount()-1,1,new QTableWidgetItem(""));
                tab->setItem(tab->rowCount()-1,2,new QTableWidgetItem(""));
                tab->setItem(tab->rowCount()-1,3,new QTableWidgetItem(ops[j]->getCompte()->getNom()));
                tab->setItem(tab->rowCount()-1,4,new QTableWidgetItem(QString::number(ops[j]->getDebit())));
                tab->setItem(tab->rowCount()-1,5,new QTableWidgetItem(QString::number(ops[j]->getCredit())));
            }
            // Mise en couleur en fonction de l'état de rapprochement
            QColor rapprocheFond = QColor(255,200,200);
            QColor rapprocheText = Qt::red;
            if(tm.getTransactions()[i]->isRapproche()){
                rapprocheFond = QColor(200,255,200);
                rapprocheText = Qt::green;
            }
            for(int k=0; k < tab->columnCount();k++){
                for(int m = 0; m < ops.size()+1; m++) {
                    tab->item(transactionRow+m, k)->setBackground(rapprocheFond);
                    tab->item(transactionRow+m, k)->setForeground(rapprocheText);
                }
            }
        }
}

void MainWindow::affichageTransactions(AbstractCompte* compteCourant){

    ui->tableTransactions->clearContents();
    ui->tableTransactions->setRowCount(0);
    TransactionManager& tm = TransactionManager::getInstance();
    QTableWidget* tab = ui->tableTransactions;

    /* ======================== Affichage avec filtre ============================= */

        Compte* cptSelect = static_cast<Compte*>(compteCourant);
        for(auto it = tm.getConstFilterIterator(*cptSelect); !it.isDone(); it.next()){
            int transactionRow = tab->rowCount();
            tab->setRowCount(tab->rowCount()+1);
            tab->setItem(tab->rowCount()-1,0,new QTableWidgetItem(it.currentItem()->getDate().toString("dd/MM/yyyy")));
            tab->setItem(tab->rowCount()-1,1,new QTableWidgetItem(it.currentItem()->getReference()));
            tab->setItem(tab->rowCount()-1,2,new QTableWidgetItem(it.currentItem()->getMemo()));
            //Ajout de cases vides pour la coloration non editable
            for (int i = 3; i < 7; ++i) {
                tab->setItem(tab->rowCount()-1,i,new QTableWidgetItem(""));
                tab->item(tab->rowCount()-1,i)->setFlags(tab->item(tab->rowCount()-1,i)->flags() ^ Qt::ItemIsEditable);
            }
            tab->setItem(tab->rowCount()-1,7,new QTableWidgetItem(QIcon("../icons/moins.png"),""));

            if(!it.currentItem()->isRapproche()){
                tab->item(tab->rowCount()-1,7)->setFlags(tab->item(tab->rowCount()-1,6)->flags() | Qt::ItemIsUserCheckable);
                tab->item(tab->rowCount()-1,7)->setCheckState(Qt::Unchecked);
            }
            else
                tab->item(tab->rowCount()-1,7)->setFlags(tab->item(tab->rowCount()-1,6)->flags() & ~Qt::ItemIsEnabled);

            QVector<Operation*> ops = it.currentItem()->getOperations();
            for(int j=0; j < ops.size();j++){
                tab->setRowCount(tab->rowCount()+1);
                for (int i = 0; i < 3; ++i) {
                    tab->setItem(tab->rowCount()-1,i,new QTableWidgetItem(""));
                    tab->item(tab->rowCount()-1,i)->setFlags(tab->item(tab->rowCount()-1,i)->flags() ^ Qt::ItemIsEditable);
                }
                tab->setItem(tab->rowCount()-1,3,new QTableWidgetItem(QString::number(ops[j]->getCompte()->getId())));
                tab->setItem(tab->rowCount()-1,4,new QTableWidgetItem(ops[j]->getCompte()->getNom()));
                tab->item(tab->rowCount()-1,4)->setFlags(tab->item(tab->rowCount()-1,4)->flags() ^ Qt::ItemIsEditable);
                tab->setItem(tab->rowCount()-1,5,new QTableWidgetItem(QLocale(QLocale().language(),QLocale().country()).toCurrencyString(ops[j]->getDebit())));
                tab->setItem(tab->rowCount()-1,6,new QTableWidgetItem(QLocale(QLocale().language(),QLocale().country()).toCurrencyString(ops[j]->getCredit())));
                tab->setItem(tab->rowCount()-1,7,new QTableWidgetItem(""));
                tab->item(tab->rowCount()-1,7)->setFlags(tab->item(tab->rowCount()-1,7)->flags() ^ Qt::ItemIsEditable);
            }
            // Mise en couleur en fonction de l'état de rapprochement
            QColor rapprocheFond = QColor(255,245,230);
            QColor rapprocheText = QColor(20,20,20);
            if(it.currentItem()->isRapproche()){
                rapprocheFond = QColor(230,255,230);
                rapprocheText = QColor(0,150,0);
            }
            for(int k=0; k < tab->columnCount();k++){
                for(int m = 0; m < ops.size()+1; m++) {
                    tab->item(transactionRow+m, k)->setBackground(rapprocheFond);
                    tab->item(transactionRow+m, k)->setForeground(rapprocheText);
                    if(it.currentItem()->isRapproche())
                        tab->item(transactionRow+m, k)->setFlags(tab->item(transactionRow+m, k)->flags() & ~Qt::ItemIsEditable);
                }
            }
        }
}

/* ================================================================================================================ *
 * ======================================== Modification des transactions ========================================= *
 * ================================================================================================================ */

void MainWindow::resetTransactions(){
    CompteManager& cm = CompteManager::getManager();
    QTreeWidgetItem* itemCourant = ui->arborescenceComptes->currentItem();
    if (itemCourant != nullptr)
    {
        AbstractCompte* compteCourant = cm.getCompte(ui->arborescenceComptes->currentItem()->text(1).toUInt());
        affichageTransactions(compteCourant);
    }
}

void MainWindow::modifierTransactions(){
    if(editingTransactions){
        CompteManager& cm = CompteManager::getManager();
        QTreeWidgetItem* itemCourant = ui->arborescenceComptes->currentItem();
        if (itemCourant != nullptr)
        {
            try {
                AbstractCompte* compteCourant = cm.getCompte(itemCourant->text(1).toUInt());

                TransactionManager& tm = TransactionManager::getInstance();
                QTableWidget* tab = ui->tableTransactions;
                Compte* cptSelect = static_cast<Compte*>(compteCourant);
                unsigned int row = 0;
                QVector<Transaction*> transactionASupprimer;

                for(auto it = tm.getFilterIterator(*cptSelect); !it.isDone(); it.next())
                {
                    if (it.currentItem()->isRapproche())
                    {
                        unsigned int nb_op = it.currentItem()->getOperations().size();
                        for(unsigned int j=0; j < nb_op;j++)
                            row++;
                    }
                    else if(tab->item(row,7)->checkState() == Qt::Unchecked){
                        /* ========================== Modification des infos de la transaction ========================== */

                        QString nouvelleRef = tab->item(row,1)->text();
                        QDate nouvelleDate = QDate::fromString(tab->item(row,0)->text(),"dd/MM/yyyy");
                        QString nouveauMemo = tab->item(row,2)->text();

                        /* ================= Modification des opérations après vérification de validite ================= */

                        unsigned int nb_op = it.currentItem()->getOperations().size();
                        QVector<Operation> nouvellesOps;
                        for(unsigned int j=0; j < nb_op;j++){
                            row++;
                            Compte* _cpt = static_cast<Compte*>(cm.getCompte(tab->item(row,3)->text().toUInt()));
                            bool ok;
                            double _debit = QLocale(QLocale().language(),QLocale().country()).toDouble(tab->item(row,5)->text().remove(QRegExp("[€ ]")),&ok);
                            if(!ok)
                                throw TresorerieException("La valeur du débit ne correspond pas à une somme");
                            double _credit = QLocale(QLocale().language(),QLocale().country()).toDouble(tab->item(row,6)->text().remove(QRegExp("[€ ]")),&ok);
                            if(!ok)
                                throw TresorerieException("La valeur du crédit ne correspond pas à une somme");
                            nouvellesOps.push_back(Operation(_cpt,_debit,_credit));
                        }
                        tm.modifierTransaction(it.currentItem(), nouvelleRef, nouvelleDate, nouveauMemo, nouvellesOps);
                    }
                    else
                    {
                        unsigned int nb_op = it.currentItem()->getOperations().size();
                        for(unsigned int j=0; j < nb_op;j++)
                            row++;
                        transactionASupprimer.push_back(it.currentItem());
                    }
                    row++;
                }
                if(transactionASupprimer.size() > 0){
                    QString text = "Etes vous sûr de vouloir supprimer " + QString::number(transactionASupprimer.size()) + " transactions ?";
                    int reponse = QMessageBox::question(this,"Suppression de transactions",text);
                    if(reponse == QMessageBox::Yes){
                        for (int i = 0; i < transactionASupprimer.size(); ++i) {
                            tm.supprimerTransaction(transactionASupprimer[i]->getReference());
                        }
                    }
                }
                editingTransactions = false;
                affichageTransactions(compteCourant);
                affichageOperations(compteCourant);
                afficherTousLesComptes();
                //itemCourant->setSelected(true);
                setHasSaved(false);
            }
            catch(TresorerieException& e) {QMessageBox::critical(this, "Erreur", e.getInfo());}
        }
    }
    else{
        QMessageBox::critical(this, "Erreur", "Validation non effectuée : aucunes modifications réalisées");
    }
}

void MainWindow::setEditingTransactionsTrue(){
    editingTransactions = true;
}

