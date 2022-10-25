#include "CompteAbstrait.h"
#include "Compte.h"
#include <QVectorIterator>

unsigned int AbstractCompte::getProfondeur() const
{
    int i=0; CompteVirtuel* tmp = parent;
    while(tmp != nullptr){
        i++;
        tmp = &(tmp->getParent());
    }
    return i;
}

void CompteVirtuel::calculerSolde() {
    soldeCumule = 0;
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        soldeCumule += sousComptes[i]->getSolde();
    }
    if (parent != nullptr)
        parent->calculerSolde();
}

AbstractCompte* CompteVirtuel::getCompte(const unsigned int& ref) {
    if (this->id == ref)
        return this;

    for(int i=0 ; i<sousComptes.size() ; i++)
    {
       if (sousComptes[i]->getId() == ref)
           return sousComptes[i];
       if (sousComptes[i]->isVirtuel())
       {
           CompteVirtuel* compte = static_cast<CompteVirtuel*>(sousComptes[i]);
           AbstractCompte* resultat = compte->getCompte(ref); //on cherche aussi parmi les sous comptes plus profond dans la hiérarchie
           if (resultat != nullptr)
               return resultat;
       }
    }
    return nullptr;
}

AbstractCompte* CompteVirtuel::addSousCompte(const unsigned int& ref, const QString& nom, const bool& virtuel, const TypeCompte& t)
{
    if (this->type != racine && t != this->type)
        throw TresorerieException("Creation d'un compte avec un type different de son parent");
    AbstractCompte* cpt;
    if (virtuel)
        cpt = new CompteVirtuel(ref, nom, t, this);
    else
        cpt = new Compte(ref, nom, t, this);

    sousComptes.push_back(cpt);
    return cpt;
}

void CompteVirtuel::removeCompte(const unsigned int& id) {
    for(int i=0; i < sousComptes.size(); i++){
        if(sousComptes[i]->getId() == id){
            sousComptes.remove(i); //faut pas delete sousComptes[i] avant ?
            break;
        }
    }
}

double CompteVirtuel::getSoldePeriode(QDate d1, QDate d2) const {
    double soldeP = 0;
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        soldeP += sousComptes[i]->getSoldePeriode(d1,  d2);
    }
    return soldeP;
}
bool CompteVirtuel::hasTransactionsRapprochees() const
{
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        if (sousComptes[i]->hasTransactionsRapprochees())
            return true;
    }
    return false;
}

bool CompteVirtuel::hasAllTransactionsRapprochees() const
{
    int nb = 0;
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        if (sousComptes[i]->hasAllTransactionsRapprochees())
            nb++;
    }
    if (nb==sousComptes.size())
        return true;
    else
        return false;
}

void CompteVirtuel::rapprocher()
{
    save(); //ajout memento dans CareTaker
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        sousComptes[i]->rapprocher();
    }
}

CompteVirtuel::~CompteVirtuel()
{
    while(!sousComptes.empty()){
         delete sousComptes.first();
    }
    if (parent != nullptr) //cas du compte racine
        parent->removeCompte(id);
    delete historique;
}
