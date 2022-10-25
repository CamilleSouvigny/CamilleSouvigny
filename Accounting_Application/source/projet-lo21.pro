QT += core gui xml
QT += xmlpatterns
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    Compte.cpp \
    CompteAbstrait.cpp \
    CompteManager.cpp \
    fenetrereleve.cpp \
    fenetresoldeinitial.cpp \
    filemanager.cpp \
    interfaceFichier.cpp \
    operation.cpp \
    transaction.cpp \
    transactionmanager.cpp \
    mainwindow.cpp \
    interfaceCompte.cpp \
    interfaceTransaction.cpp \
    fenetrecloture.cpp

HEADERS += \
    Compte.h \
    CompteAbstrait.h \
    CompteManager.h \
    RapprochementMemento.h \
    Solde.h \
    declaration.h \
    fenetrereleve.h \
    fenetresoldeinitial.h \
    filemanager.h \
    operation.h \
    transaction.h \
    transactionmanager.h \
    mainwindow.h \
    fenetrecloture.h

FORMS += \
    fenetrereleve.ui \
    fenetresoldeinitial.ui \
    mainwindow.ui \
    fenetrecloture.ui
