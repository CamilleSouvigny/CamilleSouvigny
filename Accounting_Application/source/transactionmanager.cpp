#include "transactionmanager.h"
#include "transaction.h"
#include "operation.h"
#include "CompteManager.h"
TransactionManager* TransactionManager::instance=nullptr;

void TransactionManager::libererInstance(){
    // sauvegarde des transactions dans le xml

    // On supprime toutes les opérations
    for(int i=0; i < transactions.size(); i++){
        delete transactions[i];
    }
    if(instance != nullptr)
        delete instance;
    instance = nullptr;
}

TransactionManager& TransactionManager::getInstance()
{
    if(instance == nullptr)
        instance = new TransactionManager();
    return *instance;
}

Transaction* TransactionManager::creerTransaction(const QString &_ref, const QDate &_date, const QString& _memo, const QVector<Operation *> &_operations)
{
    try {
        checkValiditeOperations(_operations);
        checkValiditeRef(_ref);
    } catch (TresorerieException& e) {
        for (int i=0 ; i<_operations.size() ; i++)
            delete _operations[i];
        throw; //relance l'exception
    }

    Transaction* tr = new Transaction(_ref,_date,_memo,_operations);
    for(int i=0; i < tr->getOperations().size(); i++){
        tr->getOperations()[i]->setTransaction(tr);
    }

    for(int i=0; i < _operations.size(); i++){
        //Ajout de l'opération dans le compte qui correspond et maj du solde
        _operations[i]->getCompte()->ajouterOperation(_operations[i]);
        _operations[i]->getCompte()->debiter(_operations[i]->getDebit());
        _operations[i]->getCompte()->crediter(_operations[i]->getCredit());
    }

    if(transactions.isEmpty())
        transactions.append(tr);
    else{
        int i = 0;
        while(i < transactions.size() && transactions[i]->getDate() > tr->getDate())
            i++;
        transactions.insert(i,tr);
    }

    return tr;
}

void TransactionManager::checkValiditeRef(const QString &ref) const
{
    if(ref=="")
        throw TresorerieException("Il est impératif de rentrer une référence pour la transaction");

    for(int i=0; i < transactions.size(); i++)
    {
        if(transactions[i]->getReference()==ref)
            throw TresorerieException("Une transaction possède déjà cette référence : " + ref);
    }
}

void TransactionManager::checkValiditeOperations(const QVector<Operation *> &operations) const {
    //On vérifie que les comptes sont tous différents
    for(int i=0; i < operations.size();i++){
        for(int j=i+1; j < operations.size();j++){
            if(operations[i]->getCompte()->getId() == operations[j]->getCompte()->getId())
                throw TresorerieException("Une transaction ne peut impliquer deux fois le même compte !");
        }
    }

    if(operations.size() >= 2)
    {
            //calcul somme des credits - somme des débits
            double result=0;
            for(int i=0;i<operations.size();i++){
                result=result - operations[i]->getDebit() + operations[i]->getCredit();
            }

            //si somme credit - somme debit = 0 =>Transaction valide
            if(result!=0)
                throw TresorerieException("Transaction non valide. La somme des débits différente à la somme des crédits.");
    }
    else
        throw TresorerieException("Transaction non valide. Le nombre d'opérations doit être supérieur ou égal à 2");
}

void TransactionManager::checkValiditeOperations(const QVector<Operation> &operations) const {
    //On vérifie que les comptes sont tous différents
    for(int i=0; i < operations.size();i++){
        for(int j=i+1; j < operations.size();j++){
            if(operations[i].getCompte()->getId() == operations[j].getCompte()->getId())
                throw TresorerieException("Une transaction ne peut impliquer deux fois le même compte !");
        }
    }

    if(operations.size() >= 2)
    {
            //calcul somme des credits - somme des débits
            double result=0;
            for(int i=0;i<operations.size();i++){
                result=result - operations[i].getDebit() + operations[i].getCredit();
            }

            //si somme credit - somme debit = 0 =>Transaction valide
            if(result!=0)
                throw TresorerieException("Transaction non valide. La somme des débits différente à la somme des crédits.");
    }
    else
        throw TresorerieException("Transaction non valide. Le nombre d'opérations doit être supérieur ou égal à 2");
}

void TransactionManager::FilterIterator::goToTheNextTransaction()
{
    bool flag = false;
    while (!isDone())
    {
        QVector<Operation*> operations = tm.transactions[index]->getOperations();
        for (int i=0 ; i<operations.size() ; i++)
        {
            if( *(operations[i]->getCompte()) == compte) {
                flag = true; break;
            }
        }
        if (flag)
            break;
        index++;
    }
}

void TransactionManager::ConstFilterIterator::goToTheNextTransaction()
{
    bool flag = false;
    while (!isDone())
    {
        QVector<Operation*> operations = tm.transactions[index]->getOperations();
        for (int i=0 ; i<operations.size() ; i++)
        {
            if( *(operations[i]->getCompte()) == compte) {
                flag = true; break;
            }
        }
        if (flag)
            break;
        index++;
    }
}

void TransactionManager::removeTransaction(QString ref){

    for(int i=0; i < transactions.size(); i++){
        if(transactions[i]->getReference() == ref){
            transactions.remove(i);
        }
    }
}

void TransactionManager::supprimerTransaction(QString ref){
    for(int i=0; i < transactions.size(); i++){
        if(transactions[i]->getReference() == ref){
            delete transactions[i];
        }
    }
}
Transaction* TransactionManager::getTransaction(QString ref) const{
    for(int i=0; i < transactions.size(); i++){
        if(transactions[i]->getReference() == ref){
            return transactions[i];
        }
    }
    throw TresorerieException("Référence de la transaction inexistante");
}

void TransactionManager::modifierTransaction(Transaction* tr, const QString& nouvelleRef, const QDate& date, const QString& nouveauMemo, const QVector<Operation> &newOps)
{
    if (nouvelleRef != tr->ref) {
        checkValiditeRef(nouvelleRef);
        tr->setReference(nouvelleRef);
    }

    if (date != tr->date){
        if(date.isValid())
            tr->setDate(date);
        else
            throw TresorerieException("Date invalide");
    }

    if (nouveauMemo != tr->memo)
        tr->setMemo(nouveauMemo);

    modifierOperations(newOps, tr);
}

void TransactionManager::modifierOperations(const QVector<Operation> &newOps, Transaction* tr) {
    checkValiditeOperations(newOps);
    for(int j=0; j < tr->getOperations().size();j++){   
        Operation* op = tr->getOperations()[j];
        //if(newOps[j] != *op)
        //{
            op->getCompte()->debiter(op->getCredit()); //maj le solde de l'ancien compte
            op->getCompte()->crediter(op->getDebit());
            if(op->getCompte() != newOps[j].getCompte()){
                op->getCompte()->removeOperation(op->getId()); //retire l'operation de l'ancien compte
                op->setCompte(*newOps[j].getCompte()); //met à jour le compte de l'opération
                op->setId(newOps[j].getId()); //met à jour l'id de l'opération
                op->getCompte()->ajouterOperation(op); //ajoute l'opération au nouveau compte
            }
            op->setDebit(newOps[j].getDebit());
            op->setCredit(newOps[j].getCredit());
            op->getCompte()->debiter(op->getDebit()); //maj le solde du compte
            op->getCompte()->crediter(op->getCredit());
        //}
    }
}
