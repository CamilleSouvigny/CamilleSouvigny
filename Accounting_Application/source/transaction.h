#ifndef TRANSACTION_H
#define TRANSACTION_H


#include "operation.h"
#include <QString>
#include <QDate>
#include <QVector>

class Operation;

/**
  @class  Transaction "transaction.h"
  @brief Transaction contient tous les infos d'une transaction depuis des comptes émetteurs vers des comptes récepteurs, en utilisant la comptabilité en partie double.
  @author philomène Lelong
*/
class Transaction {
private:
    friend class TransactionManager;
    /**
      * @var ref
      * @brief référence de la transaction
      * @var date
      * @brief Date de la transaction
      * @var operations
      * @brief Tableau dynamique des opérations de la transaction
      * @var rapproche
      * @brief Valeur (vrai ou faux) qui indique si la transaction est rapprochée ou non
      */
    QString ref;
    QDate date;
    QString memo;
    QVector<Operation*> operations;
    bool rapproche;

    /**
      * Opérateur de recopie et Opérateur d'affectation par défaut supprimés.
      *@brief On ne peut pas avoir deux fois exactement le même compte
      */
    Transaction(const Transaction& t) = delete;
    Transaction& operator=(const Transaction& t) = delete;

    /*! @fn constructeur
     * @brief prend en paramètre
     * @brief la référence de la transaction de type QString,
     * @brief la date de la transaction de type QDate,
     * @brief un memo de type QString&
     * @brief le tableau des opérations à réaliser
     * */
    Transaction(QString _ref, QDate _date, const QString& _memo, QVector<Operation*> _operations) :
        ref(_ref), date(_date) , memo(_memo), operations( _operations), rapproche(false) {}


    //setters
    void setReference(QString _ref);
    void setDate(QDate _d);
    void setMemo(const QString& _m);

    /*! destructeur de transaction*/
    ~Transaction();

public:

    //getters
    /*!
     * @fn getReference
     * @brief renvoie la référence d'une transaction
    */
    QString getReference() const { return ref; }

    /*!
     * @fn getDate
     * @brief renvoie la date d'une transaction
    */
    QDate getDate() const { return date; }

    /*!
     * @fn isRapproche
     * @brief revoie true si la transaction est rapprochée
    */
    bool  isRapproche() const { return rapproche; }

    /*!
     * @fn getMemo
     * @brief renvoie le mémo d'une transaction
    */
    const QString& getMemo() const { return memo;  }

    /*!
     * @fn getOperations
     * @brief retourne un tableau de pointeur sur toute les opérations contenue dans la transaction
    */
    const QVector<Operation*>& getOperations() const {return operations; }

    /*!
      * @fn setRapproche
      * @brief Rendre la transaction immuable.
      */
    void setRapproche() { rapproche = true; }
};

std::ostream& operator<<(std::ostream& f, const Transaction& t); // Toujours utile ?

#endif // TRANSACTION_H
