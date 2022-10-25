#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include "declaration.h"
#include "operation.h"
#include <QString>
#include <QDate>
#include <QVector>
#include "transaction.h"

/*!
 * @class TransactionManager transactionmanager.h
 * @brief TransactionManager gère les transactions
*/
class TransactionManager
{
    /**
      * @var instance
      * @brief L'instance unique de TransactionManager
      * @var transactions
      * @brief Tableau dynamique des transations gérées par le TransactionManager
      */
    friend class CompteManager;
    static TransactionManager* instance;
    QVector<Transaction*> transactions;

    /*! constructeur et destructeur par défaut */
    TransactionManager() = default;
    ~TransactionManager() = default;

    /*! Suppression du constructeur de recopie et de l'op d'affectation*/
    TransactionManager(TransactionManager const& tm) = delete;
    TransactionManager& operator=(TransactionManager const& tm) = delete;

    void modifierOperations(const QVector<Operation> &newOps, Transaction* tr);

public:
    // getters
    /*! Crée (si besoin) et retourne l'instance unique de TransactionManager */
    static TransactionManager& getInstance();
    /*! Retourne le tableau dynamique des transactions */
    QVector<Transaction*> getTransactions() const {return transactions;}
    Transaction* getTransaction(QString ref) const;



    // méthodes
    void libererInstance();
    /*! fait appel au destructeur de la classe Transaction*/
    void supprimerTransaction(QString ref);
    /*! supprime le pointeur vers la transaction qui est suprrimé dans la tables de transaction*/
    void removeTransaction(QString ref);
    /*! Créer une transaction et ses opérations et l'ajoute au tableau dynamique transactions */
    Transaction* creerTransaction(const QString& _ref, const QDate& _date, const QString& _memo, const QVector<Operation*>& _operations);

    /*! Vérifie qu'une transaction est valide*/
    void checkValiditeOperations(const QVector<Operation*>& operations) const;
    void checkValiditeOperations(const QVector<Operation> &operations) const;
    /*! verifie la validité de la référence : si elle n'est pas déjà utilisée*/
    void checkValiditeRef(const QString& ref) const;
    /*! permet de modifier une transaction non rapprochée*/
    void modifierTransaction(Transaction* tr, const QString& nouvelleRef, const QDate& date, const QString& nouveauMemo, const QVector<Operation> &newOps);

    /*!
     * @class FilterIterator transactionmanager.h
     * @brief FilterIterator permet de parcourir les transactions du compte entré en paramètre
      */
    class FilterIterator {
    private:
        friend TransactionManager;
        /**
          * @var tm
          * @brief L'instance unique de TransactionManager
          * @var compte
          * @brief une référence constante d'un compte concret
          * @var index
          * @brief index de la transaction au sein du compte
          */
        /* === attributs === */
        const TransactionManager& tm = TransactionManager::getInstance();
        const Compte& compte;
        int index = 0;
        /* === methode ===*/
        /*! permet de passer à la transaction suivante, incrémente l'index*/
        void goToTheNextTransaction();
        /* === constructeur === */
        /*! constructeur de FilterIterator prend en paramètre une référence constante vers un compte*/
        FilterIterator(const Compte& c) : compte(c) { goToTheNextTransaction(); }
    public:
        /*! retourne true si on est à la fin du tableau de transaction du compte*/
        bool isDone() const { return index == tm.transactions.size(); }
        /*! permet de passer à la transaction suivante*/
        void next() {
            if (isDone()) throw TresorerieException("erreur, next sur un iterateur en fin de sequence d'iteration");
            index++; goToTheNextTransaction();
        }
        /*! permet d'accéder à la transaction courrante, retourne un pointeur sur une transaction*/
        Transaction* currentItem() const {
            if (isDone()) throw TresorerieException("erreur, indirection sur un iterateur en fin de sequence d'iteration");
            return tm.transactions[index];
        }
    };

    /*!
     * @class ConstFilterIterator transactionmanager.h
     * @brief ConstFilterIterator permet de parcourir les transactions du compte entré en paramètre
      */

    class ConstFilterIterator {
    private:
        friend TransactionManager;
        /**
          * @var tm
          * @brief L'instance unique de TransactionManager
          * @var compte
          * @brief une référence constante d'un compte concret
          * @var index
          * @brief index de la transaction au sein du compte
          */
        /* === attributs === */
        const TransactionManager& tm = TransactionManager::getInstance();
        const Compte& compte;
        int index = 0;
        /* === methode ===*/
        /*! permet de passer à la transaction suivante, incrémente l'index*/
        void goToTheNextTransaction();
        /* === constructeur === */
        ConstFilterIterator(const Compte& c) : compte(c) { goToTheNextTransaction(); }
    public:
        /*! retourne true si on est à la fin du tableau de transaction du compte*/
        bool isDone() const { return index == tm.transactions.size(); }
        /*! permet de passer à la transaction suivante*/
        void next() {
            if (isDone()) throw TresorerieException("erreur, next sur un iterateur en fin de sequence d'iteration");
            index++; goToTheNextTransaction();
        }
        /*! permet d'accéder à la transaction courrante, retourne un pointeur constant sur une transaction*/
        const Transaction* currentItem() const {
            if (isDone()) throw TresorerieException("erreur, indirection sur un iterateur en fin de sequence d'iteration");
            return tm.transactions[index];
        }
    };

    /*! permettent de parcourir les transactions du compte entré en paramètre depuis un objet transactionManager */
    FilterIterator getFilterIterator(const Compte& c) const { return FilterIterator(c); }
    ConstFilterIterator getConstFilterIterator(const Compte& c) const { return ConstFilterIterator(c); }
};

#endif // TRANSACTIONMANAGER_H
