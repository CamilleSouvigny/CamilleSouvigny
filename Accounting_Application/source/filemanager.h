#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include "CompteManager.h"
#include "transactionmanager.h"

/**
  @class  FileManager "filemanager.h"
  @brief FileManager s'occupe d'importer et exporter les données des projets de l'utilisateur en format de ficher XML. L'instance de la classe est singleton.
*/
class FileManager
{
private:
    static FileManager* instance;
    CompteManager& cm = CompteManager::getManager();
    TransactionManager& tm = TransactionManager::getInstance();
    QString nomFichier = "";

    FileManager() = default;
    ~FileManager() = default;

    /**
      *@brief Opérateur de recopie et Opérateur d'affectation par défaut supprimés.
      */
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;


    void sauvegarderCompte(QDomDocument& doc) const;
    void sauvegarderTransaction(QDomDocument& doc) const;
    void sauvegarderOperations(QDomDocument& doc, QDomElement& nodeTransaction, Transaction* transaction) const;
    void sauvegarderGenerateurs(QDomDocument& doc) const;

    /**
     * @fn sauvegarderHistorique
     * @brief permet d'exporter l'historiques des rapprochements (mementos) de chaque compte de l'application dans un fichier XML
     * @exception Le compte racine n'a pas d'historique
     */
    void sauvegarderHistorique (QDomDocument& doc, QDomElement& comptenode, const AbstractCompte* c) const;
    void importerCompte(QDomDocument& doc) const;
    void importerTransactions(QDomDocument& doc) const;

    /**
     * @fn importerOperations
     * @brief listeOperations permet d'importer une opérations liée à une transaction à partir d'un fichier XML
     * @param transacnode : noeud de la transaction à laquelle l'opération est associée
     */
    QVector<Operation*> importerOperations(QDomElement& transacnode) const;

    /**
     * @fn importerGenerateurs
     * @brief permet de  sauvegarder le contexte (les propriétés qui vont permettre de récuperer les paramètres présents lors de la dernière exécution.
     */
    void importerGenerateurs(QDomDocument& doc) const;

    /**
      * @fn importerHistorique
      * @brief Permet d'importer les historiques des rapprochements d'un compte d'un fichier XML
      * @param comptenode,c : comptenode est le noeud du compte abstrait c
      */
    void importerHistorique(QDomElement& comptenode, AbstractCompte* c) const;

public:
    /**
     * @fn getFileManager
     * @brief Renvoyer l'instance singleton du FileManager. On ne peut pas avoir deux Managers des fichiers en même temps.
     */
    static FileManager& getFileManager() { if(instance == nullptr) instance = new FileManager(); return *instance; }
    const QString& getNomFichier() const { return nomFichier; }

    /**
     * @fn creeProjet
     * @brief Créer un nouveau projet de trésorerie.
     */
    void creerProjet();

    /**
     * @fn importerProjet
     * @brief Importer un projet d'un fichier XML externe (importe les comptes, historique des rapprochements, transations, opérations et générateurs)
     */
    void importerProjet(const QString& filename);

    /**
     * @fn sauvegarderProjet
     * @brief Sauvegarder le projet actuel dans un fichier XML.
     */
    void sauvegarderProjet();
};

#endif // FILEMANAGER_H
