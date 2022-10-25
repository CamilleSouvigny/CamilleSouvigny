#include "operation.h"
#include <QMessageBox>

Operation::Operation(Compte* cp, double d, double c): compte(cp), debit(d), credit(c){
    id=cp->getIdSuivantOperation();
    checkValidite();
}

//methodes
void Operation::checkValidite()
{
    //case when both credit and debit are zero
    if(credit==0 && debit==0)
        throw TresorerieException("Opération non valide : Crédit et Débit sont nuls");

    if(credit<debit)
    {
        debit-=credit;
        credit=0;
    }
    else
    {
        credit-=debit;
        debit=0;
    }
}

Operation::~Operation(){

}
