#include "CompteManager.h"
#include <QVectorIterator>
#include<filemanager.h>
#include <QPrinter>
#include <QTextDocument>

CompteManager* CompteManager::instance=nullptr;
unsigned int CompteManager::prochainId = 1;
unsigned int CompteManager::prochaineRefSoldeInitial = 1;

void CompteManager::supprimerTousLesComptes()
{
    delete compteRacine;
    compteRacine = nullptr;
    compteRacine = new CompteVirtuel(0, "Compte Racine", racine);
    prochainId = 1;
}

void CompteManager::supprimerCompte(AbstractCompte *cpt)
{
    if (cpt->getId() == 0)
        supprimerTousLesComptes();
    else
        delete cpt;
}

CompteManager& CompteManager::getManager(){
    if(instance == nullptr)
        instance = new CompteManager();
    return *instance;
}

AbstractCompte* CompteManager::creerCompte(const QString& nom, const TypeCompte& type, const bool& virtuel, const unsigned int& idParent, const unsigned int& id)
{
    if (isIdAlreadyUsed(id))
        throw TresorerieException("Impossible d'utiliser cet id de compte : il existe déjà");

    AbstractCompte* nouveauCompte;
    if (idParent == 0)
        nouveauCompte = compteRacine->addSousCompte(id, nom, virtuel, type);
    else
    {
        CompteVirtuel* parent = static_cast<CompteVirtuel*>(compteRacine->getCompte(idParent));
        if (parent == nullptr)
            throw TresorerieException("Impossible de trouver le compte parent");

        nouveauCompte = parent->addSousCompte(id, nom, virtuel, type);
    }

    nouveauCompte->save();
    prochainId++;
    return nouveauCompte;
}

AbstractCompte* CompteManager::creerCompte(const QString& nom, const TypeCompte& type, const unsigned int& idParent, Compte* capitauxPropre, double soldeInitial)
{
    if (capitauxPropre->getType() != passif)
        throw TresorerieException("Le compte de capitaux propre doit être un compte de passif");
    if (type != actif && type != passif)
        throw TresorerieException("Seul un compte d'actifs ou de passifs peut avoir un solde initial");

    AbstractCompte* nouveauCompte;
    if (idParent == 0)
        nouveauCompte = compteRacine->addSousCompte(prochainId, nom, false, type);
    else
    {
        CompteVirtuel* parent = static_cast<CompteVirtuel*>(compteRacine->getCompte(idParent));
        if (parent == nullptr)
            throw TresorerieException("Impossible de trouver le compte parent");

        nouveauCompte = parent->addSousCompte(prochainId, nom, false, type);
    }

    Compte* compteCree = static_cast<Compte*>(nouveauCompte);
    setSoldeInitial(compteCree, capitauxPropre, soldeInitial);

    compteCree->save();
    prochainId++;
    return compteCree;
}

void CompteManager::setSoldeInitial(Compte* nouveauCompte, Compte* capitauxPropre, double montant)
{
    TransactionManager& tm = TransactionManager::getInstance();
    QVector<Operation*> ops;
    QDate d1=QDate::currentDate();

    //générer réf unique de type "SI+no"
    QString ref="SI";
    ref.append(QString::number(prochaineRefSoldeInitial));

    TypeCompte type = nouveauCompte->getType();
    if (type == actif)
    {
        ops.push_back(new Operation(nouveauCompte, montant,0));
        ops.push_back(new Operation(capitauxPropre, 0, montant));
    }
    else
    {
        ops.push_back(new Operation(nouveauCompte, 0, montant));
        ops.push_back(new Operation(capitauxPropre, montant, 0));
    }
    tm.creerTransaction(ref, d1, "Solde initial", ops);
    prochaineRefSoldeInitial++;
}

void CompteManager::realiserCloture(const QString& refCloture, const QString& nomCloture, const QString& refConstatation, const QString& nomConstatation)
{
    TransactionManager& tm = TransactionManager::getInstance();

    if (nomCloture == "" || nomConstatation == "")
        throw TresorerieException("Veuillez entrer des noms et références valides");
    tm.checkValiditeRef(nomCloture + "D"); tm.checkValiditeRef(nomCloture + "R"); tm.checkValiditeRef(refConstatation);

    Compte* res, *deficit, *excedent;
    bool existRes=false, existDef=false, existEx=false;
    double montantDepenses=0, montantRecettes=0;
    QVector<Operation*> depenses, recettes;

    /* ============== Remise à zéro des comptes de dépenses et de recettes ================== */

    for(auto it=getIterator() ; !it.isDone() ; it.next()){
        if (!it.currentItem()->isVirtuel())
        {
            Compte* c= static_cast<Compte*>(it.currentItem());
            if (c->getType()==depense && c->getSolde()!=0){
               depenses.push_back(new Operation(c, 0, c->getSolde())); //attention id=0 pour toutes les opération
               montantDepenses+=c->getSolde();
            }
            if (c->getType()==recette && c->getSolde()!=0){
               recettes.push_back(new Operation(c,c->getSolde(),0)); //attention id=0 pour toutes les opération
               montantRecettes+=c->getSolde();
            }
            if(c->getNom()=="Resultat" && c->getType()==passif){
                res= static_cast<Compte*>(c);
                existRes=true;
            }
            else if(c->getNom()=="Deficit" && c->getType()==passif){
                deficit= static_cast<Compte*>(c);
                existDef=true;
            }
            else if(c->getNom()=="Excedent" && c->getType()==passif){
                excedent= static_cast<Compte*>(c);
                existEx=true;
            }
        }
     }

    //création du compte résultat s'il n'existe pas
    if(!existRes){
        res=static_cast<Compte*>(creerCompte("Resultat",passif, 0, compteRacine->getId()));
    }

    /* ============== Transaction de clôture ================== */

    if(montantRecettes!=0){
        recettes.push_back(new Operation(res,0,montantRecettes));
        tm.creerTransaction(refCloture + "R", QDate::currentDate(), nomCloture, recettes);
    }
    if(montantDepenses!=0){
        depenses.push_back(new Operation(res,montantDepenses,0));
        tm.creerTransaction(refCloture + "D", QDate::currentDate(), nomCloture, depenses);
    }

    /* ============ Constatation du résultat ============== */

    QVector<Operation*> ops;

    if(res->getSolde()<0){ //Cas de déficit
        if(!existDef){
            deficit=static_cast<Compte*>(creerCompte("Deficit",passif,0,compteRacine->getId()));
        }
        ops.push_back(new Operation(res,res->getSolde(),0));
        ops.push_back(new Operation(deficit,0,res->getSolde()));
        tm.creerTransaction(refConstatation, QDate::currentDate(), nomConstatation, ops);
    }
    else if(res->getSolde()>0) //Cas d'exédent
    {
        if(!existEx){
            excedent=static_cast<Compte*>(creerCompte("Excedent",passif,0,compteRacine->getId()));
        }
        ops.push_back(new Operation(res,res->getSolde(),0));
        ops.push_back(new Operation(excedent,0,res->getSolde()));
        tm.creerTransaction(refConstatation, QDate::currentDate(), nomConstatation, ops);
    }
}

bool CompteManager::isIdAlreadyUsed(unsigned int id) const
{
    for (auto it=getIterator() ; !it.isDone() ; it.next())
    {
        if (it.currentItem()->getId() == id)
            return true;
    }
    return false;
}

void CompteManager::Iterator::initCompteListe()
{
    compteListe.push_back(cm.compteRacine);
    ajouterSousComptes(cm.compteRacine->getSousComptes());
}

void CompteManager::Iterator::ajouterSousComptes(QVector<AbstractCompte *> sousComptes)
{
    for (int i=0 ; i<sousComptes.size() ; i++)
    {
        compteListe.push_back(sousComptes[i]);
        if (sousComptes[i]->isVirtuel()) {
            CompteVirtuel* c =static_cast<CompteVirtuel*>(sousComptes[i]);
            ajouterSousComptes(c->getSousComptes());
        }
    }
}

/* ================================================================================================================ *
 * =========================================== Création des documents ============================================= *
 * ================================================================================================================ */

void CompteManager::creerPDF(QString contenu){
    QString fileName=QFileDialog::getSaveFileName(nullptr,"Export PDF","","*.pdf");

    if (fileName != "")
    {
        if (QFileInfo(fileName).suffix() != "pdf") { fileName.append(".pdf"); }

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(fileName);

        QTextDocument doc;
        doc.setDefaultStyleSheet("h1 {text-align: center; }"
                                 "table { border-collapse: collapse; } "
                                 "td { padding: 2px 0 4px 0; } "
                                 "td.header { font-weight: bold; padding-bottom : 6px; border-bottom: 1px solid #ddd; } "
                                 "td.bordure { padding-top: 6px; border-top: 1px solid #ddd; }");
        doc.setHtml(contenu);
        doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
        doc.print(&printer);
    }
}

void CompteManager::genererBilan(){
    QString texte = "<h1>"+getRacine()->getNom()+"</h1>";
    texte += "<h2> Bilan du "+QDate::currentDate().toString("dd/MM/yyyy")+"</h2><br/>";
    texte+=listerComptesType(actif);
    texte+=listerComptesType(passif);
    texte+= "<p>document émis le " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") + "</p>";
    creerPDF(texte);
}

void CompteManager::genererReleve(QDate d1, QDate d2){
    QString texte = "<h1>"+getRacine()->getNom()+"</h1>";
    texte += "<h2> Relevé du " + d1.toString("dd/MM/yyyy") + " au " + d2.toString("dd/MM/yyyy") + "</h2><br/>";
    texte+=listerComptesType(recette,d1,d2);
    texte+=listerComptesType(depense,d1,d2);
    texte+= "<p>document émis le " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") + "</p>";
    creerPDF(texte);
}

void CompteManager::genererResultat(){
    double montantDepenses=0, montantRecettes=0, resultat;

    for(auto it=getIterator(); !it.isDone();it.next()) {
       AbstractCompte* c= it.currentItem();
       if (!c->isVirtuel()){
            if (c->getType()==depense && c->getSolde()!=0){
               montantDepenses+=c->getSolde();
            }
            if (c->getType()==recette && c->getSolde()!=0){
               montantRecettes+=c->getSolde();
            }
        }
    }

    resultat=montantRecettes-montantDepenses;
    QString texte = "<h1>"+getRacine()->getNom()+"</h1>";
    texte += "<h2> Résultat du "+QDate::currentDate().toString("dd/MM/yyyy")+"</h2><br/>";
    texte+=listerComptesType(recette);
    texte+=listerComptesType(depense);

    if (resultat>=0)
        texte+="<h3> Bénéfices = "+QLocale(QLocale().language(),QLocale().country()).toCurrencyString(resultat).remove(QRegExp("[()]"))+"</h3><br/>";
    else
        texte+="<h3> Déficit = "+QLocale(QLocale().language(),QLocale().country()).toCurrencyString(-resultat).remove(QRegExp("[()]"))+"</h3><br/>";
    texte+= "<p>document émis le " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") + "</p>";
    creerPDF(texte);
}

QString CompteManager::listerComptesType(TypeCompte t) const {
    double somme =0;
    QString type = convertEnumToString(t);

    QString text="<h3>"+type+":</h3>";
    text+="<table width=\"350\">";
    text+="<tr><td class=\"header\">Comptes</td><td class=\"header\">Solde</td></tr>";

    for(auto it=getIterator(); !it.isDone();it.next()){
        if (it.currentItem()->getType()==t){
            text+="<tr><td>";
            for (unsigned int i=0; i<it.currentItem()->getProfondeur()-1; i++){
                text += "&nbsp;&nbsp;&nbsp;&nbsp;"; //espaces forcés en html
            }
            text+=it.currentItem()->getNom() + "</td>";
            text+="<td>";
            if (!it.currentItem()->isVirtuel()){
                somme+=it.currentItem()->getSolde();
                text+=QLocale(QLocale().language(),QLocale().country()).toCurrencyString(it.currentItem()->getSolde()).remove(QRegExp("[()]"));
            }
            text+="</td></tr>";
        }
     }
    text+="<tr><td class=\"bordure\"><b> TOTAL "+type+":</b></td><td class=\"bordure\"><b>"+QLocale(QLocale().language(),QLocale().country()).toCurrencyString(somme).remove(QRegExp("[()]"))+"</b></td></tr>";
    text+="</table><br><br>";

    return text;
}

QString CompteManager::listerComptesType(TypeCompte t, QDate d1, QDate d2) const {

    double somme =0;
    QString type = convertEnumToString(t);

    QString text="<h3>"+type+":</h3>";
    text+="<table width=\"350\">";
    text+="<tr><td class=\"header\">Comptes</td><td class=\"header\">Solde</td></tr>";

    for(auto it=getIterator(); !it.isDone();it.next()){
        if (it.currentItem()->getType()==t){
            text+="<tr><td>";
            for (unsigned int i=0; i<it.currentItem()->getProfondeur()-1; i++){
                text += "&nbsp;&nbsp;&nbsp;&nbsp;"; //espaces forcés en html
            }
            text+=it.currentItem()->getNom() + "</td>";
            text+="<td>";
            if (!it.currentItem()->isVirtuel()){
                somme+=it.currentItem()->getSoldePeriode(d1,d2);
                text+=QLocale(QLocale().language(),QLocale().country()).toCurrencyString(it.currentItem()->getSoldePeriode(d1,d2)).remove(QRegExp("[()]"));
            }
            text+="</td></tr>";
        }
     }
    text+="<tr><td class=\"bordure\"><b> TOTAL "+type+":</b></td><td class=\"bordure\"><b>"+QLocale(QLocale().language(),QLocale().country()).toCurrencyString(somme).remove(QRegExp("[()]"))+"</b></td></tr>";
    text+="</table><br><br>";

    return text;
}


