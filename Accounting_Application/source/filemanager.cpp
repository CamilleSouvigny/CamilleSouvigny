#include "filemanager.h"
#include <QInputDialog>
#include "mainwindow.h"
#include <QXmlSchema>
#include <QXmlSchemaValidator>


FileManager* FileManager::instance=nullptr;

/* ================================================================================================================ *
 * ============================================ Creation d'un projet ============================================== *
 * ================================================================================================================ */

void FileManager::creerProjet()
{
    nomFichier = QFileDialog::getSaveFileName(nullptr, "Creer un fichier de trésorerie", "", "Fichier de trésorerie .treso (*.treso)");

    if (nomFichier == "")
        throw TresorerieException("Annulation de la creation d'un fichier");

    if (QFileInfo(nomFichier).suffix() != "treso") { nomFichier.append(".treso"); }
    cm.supprimerTousLesComptes();
    QString filename = QUrl(nomFichier).fileName();
    cm.setNomAsso(filename.remove(filename.lastIndexOf('.'), 6));
}

/* ================================================================================================================ *
 * ========================================== Importation d'un projet ============================================= *
 * ================================================================================================================ */

void FileManager::importerProjet(const QString& filename)
{
    if (filename != "")
    {
        nomFichier = filename;
        /*QXmlSchema schema;

        if (schema.load(QUrl("../schemaXML.xsd"))) {
            QFile file(nomFichier);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                throw TresorerieException("Erreur de lecture du fichier !");

            QXmlSchemaValidator validator(schema);
            if (!validator.validate(&file, QUrl::fromLocalFile(file.fileName())))
                throw TresorerieException("Veuillez entrer un fichier de projet valide");
            file.close(); //obligé de le fermer avant de l'utiliser pour autre chose
        }*/

        QFile file(nomFichier);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw TresorerieException("Erreur de lecture du fichier " + nomFichier);
        /*----------------------------------------------------------
         * Le document xml est représenté par un objet QDomDocument. L'appel à
         * setContent permet de parser le fichier xml et de créer les éléments
         * correspondants dans le QDomDocument sous une forme arborescente
         *----------------------------------------------------------*/
        QDomDocument xmlDoc;
        xmlDoc.setContent(&file);

        cm.supprimerTousLesComptes();
        importerCompte(xmlDoc);
        importerTransactions(xmlDoc);
        importerGenerateurs(xmlDoc);

        file.close();
    }
}

void FileManager::importerCompte(QDomDocument& doc) const
{
    QDomElement root = doc.documentElement();
    QDomNodeList comptes=root.elementsByTagName("comptes").item(0).toElement().elementsByTagName("compte");

    for(int i=0; i<comptes.size(); i++)
    {
        QDomElement cptnode=comptes.item(i).toElement();

        QDomNodeList list = cptnode.elementsByTagName("id");
        unsigned int id = list.item(0).toElement().text().toUInt();

        list = cptnode.elementsByTagName("nom");
        QString nom = list.item(0).toElement().text();

        if(i==0)
        {
            cm.setNomAsso(nom);
            continue;
        }

        list = cptnode.elementsByTagName("parent");
        unsigned int parentId = list.item(0).toElement().text().toUInt();

        list = cptnode.elementsByTagName("virtuel");
        bool c_virtual;
        if (list.item(0).toElement().text().toUInt() == 1)
            c_virtual = true;
        else
            c_virtual = false;

        list = cptnode.elementsByTagName("type");
        QString str_type = list.item(0).toElement().text();

        TypeCompte type = convertStringToEnum(str_type);

        cm.creerCompte(nom, type, c_virtual, parentId, id);
        importerHistorique(cptnode,cm.getCompte(id));
    }
}

void FileManager::importerHistorique(QDomElement& comptenode, AbstractCompte* c) const
{

    QDomNodeList listeHistorique = comptenode.elementsByTagName("historique").item(0).toElement().elementsByTagName("rapprochement");
    c->getHistorique().supprimerTout();

    for(int i=0; i<listeHistorique.size(); i++)
    {
        QDomElement historiquenode = listeHistorique.item(i).toElement();

        QDomNodeList list = historiquenode.elementsByTagName("solde");
        const double solde = list.item(0).toElement().text().toDouble();

        list =historiquenode.elementsByTagName("date");
        QDate date = QDate::fromString(list.item(0).toElement().text(), "dd/MM/yyyy");

         c->getHistorique().addMemento(solde,date);
    }
}

void FileManager::importerTransactions(QDomDocument &doc) const
{

    QDomElement root = doc.documentElement();
    QDomNodeList transactions=root.elementsByTagName("transactions").item(0).toElement().elementsByTagName("transaction");

    for(int i=0; i<transactions.size(); i++)
    {
        QDomElement transacnode=transactions.item(i).toElement();

        QDomNodeList list = transacnode.elementsByTagName("ref");
        QString ref = list.item(0).toElement().text();

        list = transacnode.elementsByTagName("annee");
        unsigned int annee = list.item(0).toElement().text().toInt();

        list = transacnode.elementsByTagName("mois");
        unsigned int mois = list.item(0).toElement().text().toInt();

        list = transacnode.elementsByTagName("jour");
        unsigned int jour = list.item(0).toElement().text().toInt();

        list = transacnode.elementsByTagName("memo");
        QString memo = list.item(0).toElement().text();

        list = transacnode.elementsByTagName("rapprochement");
        bool rapp = list.item(0).toElement().text().toInt();

        QVector<Operation*> listeOperations = importerOperations(transacnode);

        QDate date(annee, mois, jour);
        Transaction* nouvelleTransaction = tm.creerTransaction(ref, date, memo, listeOperations);
        if (rapp) nouvelleTransaction->setRapproche();
    }
}

QVector<Operation*> FileManager::importerOperations(QDomElement& transacnode) const
{
    QDomNodeList listeOperations = transacnode.elementsByTagName("operations").item(0).toElement().elementsByTagName("operation");
    QVector<Operation*> operations;

    for(int i=0; i<listeOperations.size(); i++)
    {
        QDomElement operationNode = listeOperations.item(i).toElement();

        QDomNodeList list = operationNode.elementsByTagName("compte");
        unsigned int idcpt = list.item(0).toElement().text().toUInt();
        Compte* cpt = static_cast<Compte*>(cm.getCompte(idcpt));

        list = operationNode.elementsByTagName("credit");
        const double credit = list.item(0).toElement().text().toDouble();

        list = operationNode.elementsByTagName("debit");
        const double debit = list.item(0).toElement().text().toDouble();

        list = operationNode.elementsByTagName("idOp");
        //unsigned int idOperation = list.item(0).toElement().text().toUInt();

        Operation* op = new Operation(cpt, debit, credit);
        operations.push_back(op);
    }
    return operations;
}

void FileManager::importerGenerateurs(QDomDocument &doc) const
{
    QDomElement generateurCompte = doc.elementsByTagName("genCompteID").item(0).toElement();
    unsigned int prochainId = generateurCompte.attribute("valeur").toUInt();
    cm.prochainId = prochainId;

    QDomElement generateurRef = doc.elementsByTagName("genRefSoldeInit").item(0).toElement();
    unsigned int prochainRef = generateurRef.attribute("valeur").toUInt();
    cm.prochaineRefSoldeInitial = prochainRef;
}

/* ================================================================================================================ *
 * =========================================== Sauvegarde d'un projet ============================================= *
 * ================================================================================================================ */

void FileManager::sauvegarderProjet()
{
    if (nomFichier == "")
        nomFichier = QFileDialog::getSaveFileName(nullptr, "Sauvegarder un fichier de trésorerie", "", "Fichier de trésorerie .treso (*.treso)");

    if (nomFichier != "")
    {
        QDomDocument doc;
        QDomElement root = doc.createElement("projet");
        doc.appendChild(root);

        sauvegarderCompte(doc);
        sauvegarderTransaction(doc);
        sauvegarderGenerateurs(doc);

        QFile outfile(nomFichier);
        if (!outfile.open(QIODevice::WriteOnly))
            return;

        QTextStream stream(&outfile);
        stream << doc.toString();
        outfile.close();
    }

}

void FileManager::sauvegarderCompte(QDomDocument& doc) const {

    QDomElement root = doc.documentElement();
    QDomElement comptes = doc.createElement("comptes");
    root.appendChild(comptes);

    for (auto it = cm.getIterator(); !it.isDone(); it.next())
    {
        const AbstractCompte* cpt=it.currentItem();
        QDomElement compteNode = doc.createElement("compte");

        QDomElement idCpt = doc.createElement("id");
        idCpt.appendChild(doc.createTextNode(QString::number(cpt->getId())));

        QDomElement nomCpt = doc.createElement("nom");
        nomCpt.appendChild(doc.createTextNode(cpt->getNom()));

        TypeCompte type=cpt->getType();
        QString str_type = convertEnumToString(type);

        QDomElement typeCpt = doc.createElement("type");
        typeCpt.appendChild(doc.createTextNode(str_type));

        QDomElement parentCpt = doc.createElement("parent");
        if(str_type=="Racine")
            parentCpt.appendChild(doc.createTextNode(""));
        else
            parentCpt.appendChild(doc.createTextNode(QString::number(cpt->getParent().getId())));

        QDomElement virtuelCpt = doc.createElement("virtuel");
        virtuelCpt.appendChild(doc.createTextNode(cpt->isVirtuel() ? "1" : "0"));

        compteNode.appendChild(idCpt);
        compteNode.appendChild(nomCpt);
        compteNode.appendChild(parentCpt);
        compteNode.appendChild(virtuelCpt);
        compteNode.appendChild(typeCpt);

        sauvegarderHistorique(doc, compteNode, cpt);

        comptes.appendChild(compteNode);
    }
}

void FileManager::sauvegarderHistorique (QDomDocument& doc, QDomElement& comptenode, const AbstractCompte* c) const {

    if (c->getId() != 0)
    {
        QDomElement historique = doc.createElement("historique");
        RapprochementCareTaker& historiqueCompte = c->getHistorique();

        for(auto it=historiqueCompte.cbegin() ; it!=historiqueCompte.cend() ; ++it)
        {
            RapprochementMemento* currentRapprochement = *it;
            QDomElement rapprochementnode = doc.createElement("rapprochement");

            QDomElement date = doc.createElement("date");
            date.appendChild(doc.createTextNode(currentRapprochement->getDate().toString("dd/MM/yyyy")));

            QDomElement solde = doc.createElement("solde");
            solde.appendChild(doc.createTextNode(QString::number(currentRapprochement->getSolde())));

            rapprochementnode.appendChild(solde);
            rapprochementnode.appendChild(date);

            historique.appendChild(rapprochementnode);

        }
        comptenode.appendChild(historique);
    }
}

void FileManager::sauvegarderTransaction(QDomDocument& doc) const {

    QDomElement root = doc.documentElement();
    QDomElement transactions = doc.createElement("transactions");
    root.appendChild(transactions);

    QVector<Transaction*> listeTransactions = tm.getTransactions();

    for(int i=0; i<listeTransactions.size(); i++)
    {
        Transaction* currentTransaction = listeTransactions[i];
        QDomElement transacnode = doc.createElement("transaction");

        QDomElement ref = doc.createElement("ref");
        ref.appendChild(doc.createTextNode(currentTransaction->getReference()));

        QDomElement annee = doc.createElement("annee");
        QDomElement mois = doc.createElement("mois");
        QDomElement jour = doc.createElement("jour");
        annee.appendChild(doc.createTextNode(QString::number(currentTransaction->getDate().year())));
        mois.appendChild(doc.createTextNode(QString::number(currentTransaction->getDate().month())));
        jour.appendChild(doc.createTextNode(QString::number(currentTransaction->getDate().day())));

        QDomElement memo = doc.createElement("memo");
        memo.appendChild(doc.createTextNode(currentTransaction->getMemo()));

        QDomElement rapp = doc.createElement("rapprochement");
        rapp.appendChild(doc.createTextNode(currentTransaction->isRapproche() ? "1" : "0"));

        transacnode.appendChild(ref);
        transacnode.appendChild(annee);
        transacnode.appendChild(mois);
        transacnode.appendChild(jour);
        transacnode.appendChild(memo);
        transacnode.appendChild(rapp);

        sauvegarderOperations(doc, transacnode, currentTransaction);

        transactions.appendChild(transacnode);
    }
}

void FileManager::sauvegarderOperations(QDomDocument& doc, QDomElement& nodeTransaction, Transaction* transaction) const
{
    QDomElement operations = doc.createElement("operations");

    QVector<Operation*> listeOperations = transaction->getOperations();
    for (int i=0 ; i<listeOperations.size() ; i++)
    {
        QDomElement operation = doc.createElement("operation");

        Operation* currentOp = listeOperations[i];
        QDomElement idOp = doc.createElement("idOp");
        idOp.appendChild(doc.createTextNode(QString::number(currentOp->getId())));

        QDomElement compte = doc.createElement("compte");
        compte.appendChild(doc.createTextNode(QString::number(currentOp->getCompte()->getId())));

        QDomElement credit = doc.createElement("credit");
        credit.appendChild(doc.createTextNode(QString::number(currentOp->getCredit())));

        QDomElement debit = doc.createElement("debit");
        debit.appendChild(doc.createTextNode(QString::number(currentOp->getDebit())));

        operation.appendChild(idOp);
        operation.appendChild(compte);
        operation.appendChild(debit);
        operation.appendChild(credit);
        operations.appendChild(operation);
    }
    nodeTransaction.appendChild(operations);
}

void FileManager::sauvegarderGenerateurs(QDomDocument &doc) const
{
    QDomElement root = doc.documentElement();
        QDomElement generateurs = doc.createElement("generateurs");
            QDomElement generateurCompte = doc.createElement("genCompteID");
            generateurCompte.setAttribute("valeur", cm.prochainId);
            QDomElement generateurRefSoldeInit = doc.createElement("genRefSoldeInit");
            generateurRefSoldeInit.setAttribute("valeur", cm.prochaineRefSoldeInitial);
        generateurs.appendChild(generateurRefSoldeInit);
        generateurs.appendChild(generateurCompte);
    root.appendChild(generateurs);
}
