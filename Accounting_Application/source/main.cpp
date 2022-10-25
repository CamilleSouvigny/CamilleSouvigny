#include "mainwindow.h"
#include <QApplication>
#include "Compte.h"

#include "transactionmanager.h"
#include "filemanager.h"
#include "fenetresoldeinitial.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
/*
        CompteManager& cm = CompteManager::getManager();
        cout<<cm.getRacine()->getHistorique().getDernierSolde();

*/
        /*
        AbstractCompte* act = cm.creerCompte("Actif", actif, true);
        cm.creerCompte("Passif", passif, true);
        cm.creerCompte("Depenses", depense, true);
        AbstractCompte* rec = cm.creerCompte("Recette", recette, true);

        AbstractCompte* gala = cm.creerCompte("Gala", recette, true, rec->getId());
        AbstractCompte* utceenne = cm.creerCompte("Utceenne", recette, true, rec->getId());

        AbstractCompte* ventePlaceGala = cm.creerCompte("Vente Places", recette, false, gala->getId());

        AbstractCompte* ventePlaceUtceenne = cm.creerCompte("Vente Places", recette, false, utceenne->getId());

        Compte* banque = static_cast<Compte*>(cm.creerCompte("Banque", actif, false, act->getId()));

        cm.creerCompte("testPassif", passif, 0);
        cm.creerCompte("testPassif2", passif, 0, 2);
        cm.creerCompte("passifvirtuel2", passif, 1, 2);

        // Transactions
        TransactionManager& tm = TransactionManager::getInstance();

        QVector<Operation*> ops;
        ops.push_back(new Operation(static_cast<Compte*>(banque), 22.5,0));
        ops.push_back(new Operation(static_cast<Compte*>(ventePlaceGala), 0,22.5));
        QDate d1(2019, 11, 17);
        tm.creerTransaction("1", d1, "vente place gala", ops);

        QVector<Operation*> ops1;
        ops1.push_back(new Operation(static_cast<Compte*>(banque), 10, 0));
        ops1.push_back(new Operation(static_cast<Compte*>(ventePlaceUtceenne), 0, 10));
        QDate d2(2020, 05, 05);
        tm.creerTransaction("2", d2, "vente place utceenne", ops1);

        QVector<Operation*> ops2;
        ops2.push_back(new Operation(static_cast<Compte*>(banque), 50, 0));
        ops2.push_back(new Operation(static_cast<Compte*>(ventePlaceUtceenne), 0, 50));
        QDate d3(2020, 05, 05);
        tm.creerTransaction("3", d3, "vente bieres utceenne", ops2);

         cout << "\n========= Test Rapprochement Compte ============\n\n";



         double soldeActuel = banque->getSolde();
         double dernierSolde=banque->getCareTaker()->rapprocherCompte();
         cout<<"solde actuel"<<soldeActuel<<"\n";
         cout<<"dernier solde"<<dernierSolde<<"\n";
         bool reponse=true;
             if(reponse){

                 QVector<Transaction*> tm;
                 cout<<"erre\n";
                 QVector<Transaction*> *tmp=cm.rapprocherTransactions(banque,&tm);
                 if(!tmp)
                    throw TresorerieException("Aucunes transactions à rapprocher");
                 cm.validerRapprochement(tmp);
             }
              else {
               cm.annulerRapprochement(banque);
              }

    } catch(TresorerieException& e) { cout<<e.getInfo().toStdString()<<"\n"; }*/

    MainWindow w;
    w.show();

    return a.exec();

}
