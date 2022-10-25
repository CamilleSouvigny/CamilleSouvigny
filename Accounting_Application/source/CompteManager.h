#ifndef FACADE_H
#define FACADE_H

#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include<QFileDialog>
#include<QTextStream>
#include "declaration.h"
#include "CompteAbstrait.h"
#include "transactionmanager.h"

class TransactionManager;
class RapprochementCareTaker;
class FileManager;

/**
 * @class CompteManager
 * @brief Permet de gérer l'ajout de comptes
 * @var prochainId
 * @brief identifiant utilisé pour la génération de l'id de chaque compte
 * @var prochaineRefSoldeInitial
 * @brief identifiant utilisé pour la génération des reférences uniques des transactions SoldeInitial
 * @var instance
 * @brief instance unique de compteManager
 * @var compteRacine
 */
class CompteManager {
    friend FileManager;
private:
    CompteVirtuel* compteRacine=nullptr;
    static unsigned int prochainId;
    static unsigned int prochaineRefSoldeInitial;
    static CompteManager* instance;

    CompteManager() { compteRacine = new CompteVirtuel(0, "Compte Racine", racine); }
    ~CompteManager() { delete compteRacine; }

    CompteManager& operator=(CompteManager& f)=delete;
    CompteManager (CompteManager& f)=delete;
    bool isIdAlreadyUsed(unsigned int id) const;
    void setSoldeInitial(Compte* nouveauCompte, Compte* capitauxPropre, double montant);

public:

    /**
     * @fn getManager
     * @return CompteManager&  (instance unique)
     */
    static CompteManager& getManager();
    const CompteVirtuel* getRacine() const { return compteRacine; } //il faut empecher la destruction de la racine avec delete, sinon la compte manager crash, comment faire ?
    void supprimerTousLesComptes();
    void supprimerCompte(AbstractCompte* cpt);
    AbstractCompte* getCompte(unsigned int id) const { return compteRacine->getCompte(id); }

    /**
     * @fn creerCompte
     * @brief cree un compte avec un solde initialisé à 0
     */
    AbstractCompte* creerCompte(const QString& nom, const TypeCompte& type, const bool& virtuel, const unsigned int& idParent = 0, const unsigned int& id = prochainId);

    /**
     * @fn creerCompte
     * @brief cree un compte concret d'actif ou de passif avec un solde initial correspondant au paramètre soldeInitial
     */
    AbstractCompte* creerCompte(const QString& nom, const TypeCompte& type, const unsigned int& idParent, Compte* capitauxPropre, double soldeInitial);
    void setNomAsso(const QString& nom) { compteRacine->setNom(nom); }
    QString getNomAsso() const { return compteRacine->getNom(); }
    void realiserCloture(const QString& refCloture, const QString& nomCloture, const QString& refConstatation, const QString& nomConstatation);

    QString listerComptesType(TypeCompte t) const;
    QString listerComptesType(TypeCompte t, QDate d1, QDate d2) const;
    void genererReleve(QDate d1, QDate d2);
    void genererBilan();
    void genererResultat();
    void creerPDF(QString contenu);

    class Iterator {
        friend CompteManager;
    private:
        const CompteManager& cm = CompteManager::getManager();
        QVector<AbstractCompte*> compteListe;
        int index = 0;
        Iterator() { initCompteListe(); }
        void initCompteListe();
        void ajouterSousComptes(QVector<AbstractCompte*> sousComptes);
    public:
        bool isDone() const { return index == compteListe.size(); }
        void next() {
            if (isDone()) throw TresorerieException("erreur, next sur un iterateur compte en fin de sequence d'iteration");
            index++;
        }
        AbstractCompte* currentItem() const {
            if (isDone()) throw TresorerieException("erreur, indirection sur un iterateur compte en fin de sequence d'iteration");
            return compteListe[index];
        }
    };
    Iterator getIterator() const { return Iterator(); }
};

#endif // FACADE_H
