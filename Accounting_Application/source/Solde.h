#ifndef SOLDE_H
#define SOLDE_H

#include "declaration.h"

/*!
 * @class AbstractSolde Solde.h
 * @brief Fournit une interface pour la définition du solde d'un compte
 * @author Victor Krawiec
*/
class AbstractSolde {
protected:
    double montant=0;
public:
    AbstractSolde() = default;
    double getMontant() const { return montant; } /*! Retourne le montant du solde */
    void setMontant(const double& s) { montant=s; }
    virtual void crediter(const double& credit) = 0;
    virtual void debiter(const double& debit) = 0;
    virtual ~AbstractSolde() = default;
};

/*!
 * @class SoldeEmploi
 * Definit les méthodes nécessaires à la gestion du solde d'un compte d'actif ou de dépenses
*/
class SoldeEmploi : public AbstractSolde {
public:
    /*! constructeur et destructeur par défaut */
    SoldeEmploi() = default;
    ~SoldeEmploi() = default;
    /** Permet de créditer un solde de compte d'actif ou de dépenses */
    void crediter(const double& montantACrediter) override { montant -= montantACrediter; }
    /** Permet de debiter un solde de compte d'actif ou de dépenses */
    void debiter(const double& montantADebiter) override { montant += montantADebiter; }
};

/*!
 * @class SoldeRessource
 * Definit les méthodes nécessaires à la gestion du solde d'un compte de passif ou de recettes
*/
class SoldeRessource : public AbstractSolde {
public:
    /*! constructeur et destructeur par defaut */
    SoldeRessource() = default;
    ~SoldeRessource() = default;
    /** Permet de créditer un solde de compte de passif ou de recettes */
    void crediter(const double& montantACrediter) override { montant += montantACrediter; }
    /** Permet de débiter un solde de compte de passif ou de recettes */
    void debiter(const double& montantADebiter) override { montant -= montantADebiter; }
};


#endif // SOLDE_H
