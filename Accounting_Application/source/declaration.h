#ifndef DECLARATION_H
#define DECLARATION_H

#include <fstream>
#include <QString>
#include <QTextStream>
#include <iostream>
#include <QVector>

using namespace std;

enum TypeCompte {actif, passif, recette, depense, racine};

inline TypeCompte convertStringToEnum(const QString& str)
{
    if (str == "Actif") return actif;
    else if (str == "Passif") return passif;
    else if (str == "Dépenses") return depense;
    else if (str == "Racine") return racine;
    else return recette;
}

inline QString convertEnumToString(const TypeCompte& type)
{
    if (type == actif) return "Actif";
    else if(type == passif) return "Passif";
    else if (type == depense) return "Dépenses";
    else if (type == recette) return "Recettes";
    else return "Racine";
}

inline bool convertStringToBool(const QString& str)
{
    if (str=="1") return true;
    else return false;
}

class TresorerieException : public exception {
public:
    TresorerieException(const QString& message) noexcept : info(message) {}
    ~TresorerieException() noexcept = default;
    QString getInfo() const noexcept { return info; }
    virtual const char* what() const noexcept override { return info.toStdString().c_str(); }
private:
    QString info;
};

#endif // DECLARATION_H
