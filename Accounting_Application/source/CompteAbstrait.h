#ifndef COMPTEABSTRAIT_H
#define COMPTEABSTRAIT_H

#include "declaration.h"
#include "RapprochementMemento.h"
#include <QDebug>

class CompteVirtuel;
class CompteManager;

/**
@class AbstractCompte "CompteAbstrait.h"
@class CompteVirtuel "CompteAbstrait.h"
@brief AbstractCompte n'est pas instanciable, cette classe permet d'instancier CompteVirtuel ou Compte selon la situation. CompteVirtuel est un compte composé de sous comptes et n'enregistrant aucune transaction
*/

class AbstractCompte {
    /**
     * @var parent
     * @brief parent est le compte (obligatoirement virtuel) dans lequel se trouve le compte que l'n manipule. Seul le compte Racine n'a pas de parent
     * @var type
     * @brief Les comptes ne peuvent être que de 4 types : actif, passif, dépense, recette. Il existe aussi un compte de type racine (il est unique).
     * @var virtuel
     * @brief virtuel est un flag indiquant si le compte est virtuel ou non
     */
    friend class CompteManager;
    friend class CompteVirtuel;
protected :
    const unsigned int id;
    QString nom;
    CompteVirtuel* parent = nullptr;
    const TypeCompte type;
    const bool virtuel;
    RapprochementCareTaker* historique=nullptr;

    AbstractCompte(const unsigned int& ref, const QString& n, const TypeCompte& t, CompteVirtuel* p = nullptr, bool v=false) :
        id(ref),nom(n),parent(p),type(t), virtuel(v) { historique = new RapprochementCareTaker(); }

    /**
      * Opérateur de recopie et Opérateur d'affectation par défaut supprimés.
      *@brief On ne peut pas avoir deux fois exactement le même compte
      */
    AbstractCompte(const AbstractCompte& c)=delete;
    AbstractCompte& operator=(const AbstractCompte& c)=delete;
    /**
      *Destructeur virtuel.
      *@brief Les destructeurs sont implémentés dans les classes filles : Compte et CompteVirtuel
      */
    virtual ~AbstractCompte()=default;
public:
    unsigned int getId() const {return id;}
    const QString& getNom() const {return nom;}
    CompteVirtuel& getParent() const {return *parent;}
    const TypeCompte& getType() const {return type;}
    void setNom(const QString& n) { nom = n; }
    bool isVirtuel() const { return virtuel; }
    virtual bool hasTransactionsRapprochees() const = 0;
    virtual bool hasAllTransactionsRapprochees() const = 0;
    virtual double getSolde() const = 0;
    virtual double getSoldePeriode(QDate d1, QDate d2) const = 0;
    virtual void rapprocher() = 0;
    unsigned int getProfondeur() const;
    

    /**
     *@fn save() permet de sauvegarder le solde et la date dans un memento
     */
    void save() { historique->sauvegarderSolde(new RapprochementMemento(getSolde())); }
    RapprochementCareTaker& getHistorique() const {return *historique;}
};


class CompteVirtuel : public AbstractCompte {
    /**
     * @var soldeCumule
     * @brief Somme des soldes des sous Comptes appartenant au compte virtuel
     */
    friend class CompteManager;
    double soldeCumule=0;
    QVector<AbstractCompte*> sousComptes;
    CompteVirtuel(const unsigned int& ref, const QString& nom, const TypeCompte& t, CompteVirtuel* parent=nullptr) : AbstractCompte(ref, nom, t, parent, true) {}
    ~CompteVirtuel();
public:
    QVector<AbstractCompte*> getSousComptes() const { return sousComptes; }
    AbstractCompte* getCompte(const unsigned int& ref);
    double getSolde() const override { return soldeCumule; }
    double getSoldePeriode(QDate d1, QDate d2) const override;

    bool hasTransactionsRapprochees() const override;
    bool hasAllTransactionsRapprochees() const override;
    void rapprocher() override;

    AbstractCompte* addSousCompte(const unsigned int& ref, const QString& nom, const bool& virtuel, const TypeCompte& t);
    void removeCompte(const unsigned int& id);
    /**
     * @fn ChangerSolde()
     * @brief change le solde cumulé du compte virtuel à la réception d'un signal, envoyé lorsque l'un de ses fils voit son solde modifié
     */
    void calculerSolde();
};


#endif // COMPTEABSTRAIT_H
