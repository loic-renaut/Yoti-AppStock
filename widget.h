#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QtSql>
#include <QSqlDatabase>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    // bdd
    QSqlDatabase db;

    // structure generale du programme
    QHBoxLayout* mainLayout;
    QStackedWidget* stackedWidget;

    // page_choix
    QWidget* page_choix;
    QVBoxLayout* page_choixLayout;
    QPushButton* page_choixPushButtonChercher;
    QPushButton* page_choixPushButtonRanger;

    // page_chercher
    QWidget* page_chercher;
    QVBoxLayout* page_chercherLayout;
    QGroupBox* page_chercherGroupBox;
    QFormLayout* page_chercherGroupBoxLayout;
    QLineEdit* page_chercherLineEditCodeBarres;
    QDialogButtonBox* page_chercherDialogButtonBox;

    // page_ranger
    QWidget* page_ranger;
    QVBoxLayout* page_rangerLayout;
    QGroupBox* page_rangerGroupBox;
    QFormLayout* page_rangerGroupBoxLayout;
    QLineEdit* page_rangerLineEditCodeBarresIdentification;
    QLineEdit* page_rangerLineEditCodeBarresCompartiment;
    QDialogButtonBox* page_rangerDialogButtonBox;

    // page_resultatChercher
    QWidget* page_resultatChercher;
    QVBoxLayout* page_resultatChercherLayout;
    QGroupBox* page_resultatChercherGroupBox;
    QFormLayout* page_resultatChercherGroupBoxLayout;
    QLabel* page_resultatChercherLabelPhoto;
    QLabel* page_resultatChercherLabelCodeRangee;
    QLabel* page_resultatChercherLabelCodeEtage;
    QLabel* page_resultatChercherLabelCodeIdentification;
    QLineEdit* page_resultatChercherLineEditCodeBarresIdentification;
    QDialogButtonBox* page_resultatChercherDialogButtonBox;
};

#endif // WIDGET_H
