#include "transaction.h"
#include "transactionmanager.h"

std::ostream& operator<<(std::ostream& f, const Transaction& t) {
    f << t.getMemo().toStdString() << " " << t.getReference().toStdString() << " " << t.getDate().toString().toStdString();
    return f;
}


void Transaction::setReference(QString _ref){
    if (rapproche)
        throw TresorerieException("modification impossible. Transaction rapprochée.");

    ref=_ref;
}

void Transaction::setDate(QDate _d){
    if (rapproche)
        throw TresorerieException("modification impossible. Transaction rapprochée.");

    date=_d;
}

void Transaction::setMemo(const QString& _m){
    if (rapproche)
        throw TresorerieException("modification impossible. Transaction rapprochée.");

    memo=_m;
}

//void setOperation(const QVector<Operation*>& _op); : Comment l'appliquer? Est ce qu'on le rend possible ? sous quelles conditions?

Transaction::~Transaction(){
    //On appelle les destructeurs des operations
    while(!operations.isEmpty()){
        operations.first()->getCompte()->debiter(operations.first()->getCredit());
        operations.first()->getCompte()->crediter(operations.first()->getDebit());
        operations.first()->getCompte()->removeOperation(operations.first()->getId());
        delete operations.first();
        operations.removeFirst();
    }

    //On enleve le pointeur de cette transaction dans le TM
    TransactionManager& tm = TransactionManager::getInstance();
    tm.removeTransaction(ref);
}
