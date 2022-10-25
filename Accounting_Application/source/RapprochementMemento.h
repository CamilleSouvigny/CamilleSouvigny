#ifndef RAPPROCHEMENTMEMENTO_H
#define RAPPROCHEMENTMEMENTO_H

#include "declaration.h"
#include <QDateTime>

/**
 @class RaprochementMemento "RaprochementMemento.h"
 @class RapprochementCareTaker "RaprochementMemento.h"
 @brief Memento récupère un solde et Caretaker stocke tout les Memento d'un compte donnée
 @author Camille Souvigny

 */

class RapprochementCareTaker;

class RapprochementMemento {
    friend RapprochementCareTaker; //seul le caretaker peut supprimer des sauvegardes
    /**
     * @var solde
     * @brief solde associé à un compte.
     * @var date
     * @brief date au moment du rapprochement du solde.
     */

    friend class AbstractCompte;
    double solde=0;
    QDate date;
    RapprochementMemento(const RapprochementMemento& c)=default; // delete ?
    RapprochementMemento& operator=(const RapprochementMemento& c)=default;
    ~RapprochementMemento() = default;
    RapprochementMemento(double s, QDate d):solde(s),date(d) {}
    RapprochementMemento(double s): solde(s) { date = QDate::currentDate();}
public :
    double getSolde() const { return solde; }
    QDate getDate() const { return date; }
    /**
         * @fn RapprochementMemento::setSolde()
         * @brief on set le Solde: retour à un état précédent
         * @return void
         */

};

class RapprochementCareTaker{
    friend class FileManager;
    /**
     * @var  mementos
     * @brief tableaux de pointeurs de type RapprochementMemento
     * @var compte
     * @brief compte abstrait auquel est associé l'historique de solde du CareTaker
     */
    QVector<RapprochementMemento*> listeMemento;
    void addMemento(double s, QDate d) {
        RapprochementMemento *r = new RapprochementMemento(s,d);
        listeMemento.push_back(r);}
    void supprimerTout() { for (int i=0 ; i<listeMemento.size() ; i++) delete listeMemento[i]; listeMemento.clear(); }

public:
    RapprochementCareTaker() = default; // private + friend class AbstractCompte ?
    ~RapprochementCareTaker() { supprimerTout(); } // private + friend class AbstractCompte ?
    /**
     * @fn RapprochementCareTaker::sauvegarderSolde()
     * @brief sauvergarde du RapprochementMemento à la fin du vecteur
     * @return void
     */
    void sauvegarderSolde(RapprochementMemento* r) { listeMemento.push_back(r); }
    double getDernierSolde() const { return listeMemento.back()->getSolde(); }

    QVector<RapprochementMemento*>::const_iterator cbegin() const { return listeMemento.cbegin(); }
    QVector<RapprochementMemento*>::const_iterator cend() const { return listeMemento.cend(); }
};

#endif // RAPPROCHEMENTMEMENTO_H
