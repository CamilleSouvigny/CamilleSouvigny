#include "mainwindow.h"
#include "ui_mainwindow.h"

/* ================================================================================================================ *
 * ========================================= Gestion reprise du contexte ========================================== *
 * ================================================================================================================ */

void MainWindow::chargerContexte()
{
    if(QFileInfo("settings.cfg").exists())
    {
        QFile file("settings.cfg");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::information(this, "Impossible d'ouvrir le fichier de configuration", file.errorString());
            return;
        }

        QDomDocument xmlDoc;
        xmlDoc.setContent(&file);

        cout << xmlDoc.elementsByTagName("projet").item(0).toElement().attribute("chemin").toStdString() << endl;
        const QString& cheminDernierProjet = xmlDoc.elementsByTagName("projet").item(0).toElement().attribute("chemin");

        if (cheminDernierProjet != "")
        {
            importerFichier(cheminDernierProjet);

            /* ====== formAjoutCompte ======== */
            const QDomElement& formAjoutCompte = xmlDoc.elementsByTagName("formAjoutCompte").item(0).toElement();
            ui->formAjouterCompteVirtuel->setVisible(convertStringToBool(formAjoutCompte.attribute("visible")));
            ui->nom->setText(formAjoutCompte.attribute("lineNomCompte"));
            ui->checkBoxVirtuel->setChecked(convertStringToBool(formAjoutCompte.attribute("virtuelChecked")));
            ui->valeurSoldeInitial->setValue(formAjoutCompte.attribute("solde").toDouble());

            /* ===== formAjoutTransa ======== */
            const QDomElement& formAjoutTransa = xmlDoc.elementsByTagName("formAjoutTransaction").item(0).toElement();
            ui->formTransactionMainWidget->setVisible(convertStringToBool(formAjoutTransa.attribute("visible")));
            ui->lineEditRef->setText(formAjoutTransa.attribute("ref"));
            ui->lineEditMemoTransaction->setText(formAjoutTransa.attribute("nom"));
            QDate date = QDate::fromString(formAjoutTransa.attribute("date"), "dd/MM/yyyy");
            ui->dateEditTransaction->setDate(date);
            const QDomNodeList& formsOperations = xmlDoc.elementsByTagName("formOperation");
            for (int i=0 ; i<formsOperations.size() ; i++)
            {
                QDomElement courant = formsOperations.item(i).toElement();
                ajouterFormOperation(courant.attribute("indexComboBox").toUInt(), courant.attribute("credit").toDouble(), courant.attribute("debit").toDouble());
            }

            /* ======== treeWidget =========== */
            const QDomElement& treeWidget = xmlDoc.elementsByTagName("treeWidget").item(0).toElement();
            QList<QTreeWidgetItem*> tmp = ui->arborescenceComptes->findItems(treeWidget.attribute("currentCompte"), Qt::MatchRecursive, COLONNE_ID);
            QTreeWidgetItem* currentItem;
            if (!tmp.isEmpty())
                currentItem = ui->arborescenceComptes->findItems(treeWidget.attribute("currentCompte"), Qt::MatchRecursive, COLONNE_ID).first();
            else
                currentItem = ui->arborescenceComptes->topLevelItem(0);
            ui->arborescenceComptes->setCurrentItem(currentItem);
            traitementCompteSelectionne(currentItem);

            /* ======= tabWidget ========= */
            const QDomElement& tabWidget = xmlDoc.elementsByTagName("tabWidget").item(0).toElement();
            ui->tabWidget->setCurrentIndex(tabWidget.attribute("currentTab").toInt());

            file.close();
        }
    }
}

void MainWindow::sauvegarderContexte()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("contexte");
    doc.appendChild(root);
        /* ========== chemin du projet ========== */
        QDomElement projetElement = doc.createElement("projet");
        projetElement.setAttribute("chemin", currentProject);
        QDomElement widgetsElement = doc.createElement("widgets");
            /* ========== formAjoutCompte ========== */
            QDomElement formAjoutCompte = doc.createElement("formAjoutCompte");
            formAjoutCompte.setAttribute("visible", ui->formAjouterCompteVirtuel->isVisible());
            formAjoutCompte.setAttribute("lineNomCompte", ui->nom->text());
            formAjoutCompte.setAttribute("virtuelChecked", ui->checkBoxVirtuel->isChecked());
            formAjoutCompte.setAttribute("solde", ui->valeurSoldeInitial->text());
            /* ========== formAjoutTransa ========== */
            QDomElement formAjoutTransaction = doc.createElement("formAjoutTransaction");
            formAjoutTransaction.setAttribute("visible", ui->formAjouterTransaction->isVisible());
            formAjoutTransaction.setAttribute("nom", ui->lineEditMemoTransaction->text());
            formAjoutTransaction.setAttribute("ref", ui->lineEditRef->text());
            formAjoutTransaction.setAttribute("date", ui->dateEditTransaction->date().toString("dd/MM/yyyy"));
                /* ========== formsOperation ========== */
                for (int i=0 ; i<formOperations.size() ; i++)
                {
                    QDomElement formOperation = doc.createElement("formOperation");
                    formOperation.setAttribute("indexComboBox", static_cast<QComboBox*>(formOperations[i]->itemAt(0, QFormLayout::FieldRole)->widget())->currentIndex());
                    formOperation.setAttribute("debit", static_cast<QDoubleSpinBox*>(formOperations[i]->itemAt(1, QFormLayout::FieldRole)->widget())->value());
                    formOperation.setAttribute("credit", static_cast<QDoubleSpinBox*>(formOperations[i]->itemAt(2, QFormLayout::FieldRole)->widget())->value());
                    formAjoutTransaction.appendChild(formOperation);
                }
            /* ========== treeWidget ========== */
            QDomElement treeWidget = doc.createElement("treeWidget");
            QTreeWidgetItem* currentItem = ui->arborescenceComptes->currentItem();
            if (currentItem != nullptr)
                treeWidget.setAttribute("currentCompte", currentItem->text(COLONNE_ID));
            /* ========== tabWidget ========== */
            QDomElement tabWidget = doc.createElement("tabWidget");
            tabWidget.setAttribute("currentTab", ui->tabWidget->currentIndex());
       widgetsElement.appendChild(formAjoutCompte);
       widgetsElement.appendChild(formAjoutTransaction);
       widgetsElement.appendChild(treeWidget);
       widgetsElement.appendChild(tabWidget);
    root.appendChild(projetElement);
    root.appendChild(widgetsElement);

    QFile file("settings.cfg");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, "Impossible d'ouvrir le fichier de configuration", file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream << doc.toString();
    file.close();
}

/* ================================================================================================================ *
 * ======================================= Gestion des fichiers de projet ========================================= *
 * ================================================================================================================ */

void MainWindow::on_actionCreer_un_nouveau_projet_triggered()
{
    int reponse = QMessageBox::Yes;
    if (currentProject != "" && !hasSaved)
        reponse = QMessageBox::warning(this, "Attention", "Cette action entraine la perte des informations non sauvegardées. Voulez vous continuer ?", QMessageBox::Yes | QMessageBox::No);

    if (reponse == QMessageBox::Yes)
    {
        try {
            FileManager& fm = FileManager::getFileManager();
            fm.creerProjet();
            on_actionSauvegarder_triggered();
            setHasSaved(true);
            afficherTousLesComptes();
            currentProject = fm.getNomFichier();
            /* On active toutes les actions possibles une fois le projet créé */
            activerActions();
        } catch (const TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }

    }
}

void MainWindow::on_actionImporterFichier_triggered()
{
    int reponse = QMessageBox::Yes;
    if (currentProject != "" && !hasSaved)
        reponse = QMessageBox::warning(this, "Attention", "Cette action entraine la perte des informations non sauvegardées. Voulez vous continuer ?", QMessageBox::Yes | QMessageBox::No);

    if (reponse == QMessageBox::Yes)
    {
        try {
            QString nomFichier = QFileDialog::getOpenFileName(nullptr, "Ouvrir un fichier de trésorerie", "", "Fichier de trésorerie .treso (*.treso)");
            importerFichier(nomFichier);
        } catch(TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
    }
}

void MainWindow::on_actionSauvegarder_triggered()
{
    FileManager& fm = FileManager::getFileManager();

    try {
        fm.sauvegarderProjet();
        setHasSaved(true);
    } catch(TresorerieException& e) { QMessageBox::critical(this, "Erreur", e.getInfo()); }
}


void MainWindow::importerFichier(const QString& cheminFichier)
{
    FileManager& fm = FileManager::getFileManager();
    if (cheminFichier != "")
    {
        fm.importerProjet(cheminFichier);
        afficherTousLesComptes();
        currentProject = fm.getNomFichier();
        activerActions();
        setHasSaved(true);
    }
}
