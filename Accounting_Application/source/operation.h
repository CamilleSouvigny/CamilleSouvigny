#ifndef OPERATION_H
#define OPERATION_H
#include <QDebug>
#include "Compte.h"
#include "transaction.h"

class Compte;
class Transaction;
/**
  @class Compte "Compte.h"
  @class Operation "Operation.h"
  @brief Operation permet d'enregistrer une opération de crédit ou débit concernant un Compte concret.
*/

class Operation
{
    Compte* compte;
    Transaction* transaction = nullptr;
    double debit;
    double credit;
    unsigned int id;

    /**
     * @fn Operation::checkValidite()
     * @brief Validation de l'opération:
     * un montant négatif est automatiquement transformé en montant positif pour l’opération opposée;
     * une seule valeur parmi Débit ou Crédit doit être strictement positive, l’autre valeur doit être 0;
     * sinon déclanchement d'une TresorerieException.
     * @return bool
     */
    void checkValidite();

public:
    /**
     * @brief Constructeur avec des paramètres avec des valeurs par défaut
     */
    Operation(Compte* cp, double d=0, double c=0);
    ~Operation();
    //getters
    /**
     * @brief Renvoyer une référence vers le Compte qui fait partie de l'opération
     */
    Compte* getCompte() const { return compte; }
    /**
     * @brief Renvoyer une référence vers la transaction de l'opération
     */
    Transaction* getTransaction() const { return transaction; }
    /**
     * @brief Renvoyer le montant débité
     */
    double getDebit()const { return debit; }
    /**
     * @brief Renvoyer le montant crédité
     */
    double getCredit()const { return credit; }
    /**
     * @brief Renvoyer l'id de l'opération
     */
    unsigned int getId()const { return id; }

    //setters
    /**
     * @brief Donner le Compte qui fait partie de l'operation
     */
    void setId(unsigned int _id) { id=_id; }
    /**
     * @brief Donner le Compte qui fait partie de l'operation
     */
    void setCompte(Compte& cp) { compte=&cp; }
    /**
     * @brief Donner la transaction de l'operation
     */
    void setTransaction(Transaction* tr) { transaction=tr; }
    /**
     * @brief Donner le montant débité
     */
    void setDebit(double d) { debit=d; }
    /**
     * @brief Donner le montant crédité
     */
    void setCredit(double c) { credit=c; }

    bool operator!=(const Operation& op) const {
        if (compte != op.compte || transaction != op.transaction || debit != op.debit || credit != op.credit)
            return true;
        else
            return false;
    }
};

#endif // OPERATION_H
