#include "declaration.h"
#include "Compte.h"
#include "CompteManager.h"

Compte::Compte(const unsigned int& ref, const QString& nom, const TypeCompte& type, CompteVirtuel* parent) : AbstractCompte(ref, nom, type, parent)
{

    if (type == actif || type == depense)
        solde = new SoldeEmploi();
    else if (type == passif || type == recette)
        solde = new SoldeRessource();
    else
        throw TresorerieException("instanciation d'un compte avec le type racine");
}

const Operation& Compte::getOperation(unsigned int id) const{
    //récupérer une opération avec l'id i
    // déclaration d'amitié dans classe opération donc accès aux attributs/méthodes d'Opération.
    for(int i=0; i<operations.size(); i++) {
        if(operations[i]->getId()==id) return *operations[i];
    }
throw TresorerieException("Opération inexistante");
}

void Compte::removeOperation(unsigned int idOp){
    for(int i=0; i < operations.size(); i++){
        if(operations[i]->getId() == idOp){
            operations.remove(i);
            break;
        }
    }
}

void Compte::ajouterOperation(Operation* op){
    for (int j=0 ; j<operations.size() ; j++) {
           if (op->getId() == operations[j]->getId())
                throw TresorerieException("Une opération du compte\"" + nom + "\" possède déjà cette id : " + op->getId());
    }
    if(operations.isEmpty())
        operations.push_back(op);
    else{
        int i = 0;
        while(i < operations.size() && operations[i]->getTransaction()->getDate() > op->getTransaction()->getDate())
            i++;
        operations.insert(i,op);
    }

    idSuivantOperation++;
}

double Compte::getSoldePeriode(QDate d1, QDate d2) const{
    AbstractSolde* soldeP;
    if (type == actif || type == depense)
        soldeP = new SoldeEmploi();
    else if (type == passif || type == recette)
        soldeP = new SoldeRessource();

    for(int i=0; i < operations.size(); i++){
        if(operations[i]->getTransaction()->getDate() >= d1 && operations[i]->getTransaction()->getDate() <= d2){
            soldeP->crediter(operations[i]->getCredit());
            soldeP->debiter(operations[i]->getDebit());
        }
    }
    return soldeP->getMontant();
}

bool Compte::hasTransactionsRapprochees() const
{
    for (int i=0 ; i<operations.size() ; i++)
    {
        if(operations[i]->getTransaction()->isRapproche())
            return true;
    }
    return false;
}

bool Compte::hasAllTransactionsRapprochees() const
{
    int nb=0;
    for (int i=0 ; i<operations.size() ; i++)
    {
        if(operations[i]->getTransaction()->isRapproche())
            nb++;
    }
    if (nb==operations.size())
        return true;
    else
        return false;
}

void Compte::rapprocher()
{
    if (operations.size() == 0)
        throw TresorerieException("Aucune transaction à rapprocher");
    save(); //ajout Memento dans CareTaker
    for (int i=0 ; i<operations.size() ; i++)
    {
        if (!operations[i]->getTransaction()->isRapproche())
            operations[i]->getTransaction()->setRapproche();
    }
}


Compte::~Compte(){
    while(!operations.empty()){
        Operation* op = operations.first();
        TransactionManager::getInstance().supprimerTransaction(op->getTransaction()->getReference());
    }
    delete solde;
    delete historique;
    parent->removeCompte(id);
}
