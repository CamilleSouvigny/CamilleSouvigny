#ifndef COMPTE_H
#define COMPTE_H

#include "declaration.h"
#include "Solde.h"
#include "CompteAbstrait.h"
#include "operation.h"

class Operation;
class RapprochementMemento;
/**
 @class Compte "Compte.h"
 @brief Compte est concret et contien un solde et une liste d'opérations.
 @version 1.0
 @bug non compilé
 */

class Compte : public AbstractCompte {
    /**
     * @var solde
     * @brief solde d'un compte
     * @var operations
     * @brief tableau de pt d'Opération.
     */
    friend class CompteVirtuel;
    friend class CompteManager;
    unsigned int idSuivantOperation = 1;
    AbstractSolde* solde = nullptr;
    QVector<Operation*> operations;
    Compte(const unsigned int& ref, const QString& nom, const TypeCompte& type, CompteVirtuel* parent=nullptr);
    ~Compte();
public:

    unsigned int getIdSuivantOperation() const {return idSuivantOperation;}
    bool operator==(const Compte& c) const { return c.id == this->id; } // utilisé ?
    const Operation& getOperation(unsigned int id) const; // inutilisée
    QVector<Operation*>& getOperations() { return operations; }
    double getSolde() const override { return solde->getMontant(); }
    double getSoldePeriode(QDate d1, QDate d2) const override;
    bool hasTransactionsRapprochees() const override;
    bool hasAllTransactionsRapprochees() const override;
    void rapprocher() override;
    void crediter(const double& montant) { solde->crediter(montant); if (parent != nullptr) parent->calculerSolde(); }
    void debiter(const double& montant) { solde->debiter(montant); if (parent != nullptr) parent->calculerSolde(); }
    void ajouterOperation(Operation* op);
    void removeOperation(unsigned int idOp);
};

#endif // COMPTE_H

