#include "widget.h"
#include <QDebug>
#include "utils.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    // lors du lancement de l'application, on vérifie que le dossier images/images_boites existe bien dans le repertoire actif et on le créé si necessaire
    newDir("images/images_boites");

    /*
     *
     * paramètres de connexion à la bdd
     * quand le programme sera opérationel, changer ces infos par celle du serveur bdd ovh
     *
     * */
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("operateur1");
    db.setPassword("password1");
    db.setDatabaseName("yoti_test");

    /*
     *
     * structure du programme
     *
     * */
    mainLayout = new QHBoxLayout;

    stackedWidget = new QStackedWidget;

    page_choix = new QWidget;
    page_chercher = new QWidget;
    page_ranger = new QWidget;
    page_resultatChercher = new QWidget;

    stackedWidget->addWidget(page_choix);
    stackedWidget->addWidget(page_chercher);
    stackedWidget->addWidget(page_ranger);
    stackedWidget->addWidget(page_resultatChercher);

    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    stackedWidget->setCurrentWidget(page_choix);

    /*
     *
     * page_choix
     *
     * */
    page_choixLayout = new QVBoxLayout;

    page_choixPushButtonChercher = new QPushButton("Chercher un article");
    connect(page_choixPushButtonChercher, &QPushButton::clicked, this, [&] () {
        stackedWidget->setCurrentWidget(page_chercher);
    });

    page_choixPushButtonRanger = new QPushButton("Ranger un article");
    connect(page_choixPushButtonRanger, &QPushButton::clicked, this, [&] () {
        stackedWidget->setCurrentWidget(page_ranger);
    });

    page_choixLayout->addWidget(page_choixPushButtonChercher);
    page_choixLayout->addWidget(page_choixPushButtonRanger);

    page_choix->setLayout(page_choixLayout);

    /*
     *
     * page_chercher
     *
     * */
    page_chercherLayout = new QVBoxLayout;
    page_chercher->setLayout(page_chercherLayout);

    page_chercherGroupBox = new QGroupBox();
    page_chercherGroupBox->setTitle("Chercher un article :");
    page_chercherGroupBoxLayout = new QFormLayout;
    page_chercherLineEditCodeBarres = new QLineEdit;

    page_chercherGroupBoxLayout->addRow("Code-barres identification :", page_chercherLineEditCodeBarres);
    page_chercherGroupBox->setLayout(page_chercherGroupBoxLayout);
    page_chercherLayout->addWidget(page_chercherGroupBox);

    page_chercherDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(page_chercherDialogButtonBox, &QDialogButtonBox::accepted, this, [&]() {
        if(db.open())
        {
            QString s_requeteStock = "SELECT * FROM exemplaire WHERE code_barres_exemplaire = '" + page_chercherLineEditCodeBarres->text() +"';";
            QSqlQuery requeteStock;
            if(requeteStock.exec(s_requeteStock))
            {
                // Vérifie si le code-barres d'identification est présent dans la table stock
                if(requeteStock.size() == 0)
                {
                    QMessageBox::information(this, "Erreur code-barres identification", "Aucun code-barres correspondant trouvé dans le stock !");
                }
                else
                {
                    while(requeteStock.next())
                    {
                        qDebug() << requeteStock.value("fk_code_barres_compartiment").toString();
                        QString s_requeteCompartiment = "SELECT * FROM compartiment WHERE code_barres_compartiment = '" + requeteStock.value("fk_code_barres_compartiment").toString() + "';";
                        QSqlQuery requeteCompartiment;

                        if(requeteCompartiment.exec(s_requeteCompartiment))
                        {
                            while(requeteCompartiment.next())
                            {
                                page_resultatChercherLabelCodeEtage->setText(requeteCompartiment.value("code_etage").toString());
                                page_resultatChercherLabelCodeIdentification->setText(requeteCompartiment.value("code_compartiment").toString());
                                page_resultatChercherLabelCodeRangee->setText(requeteCompartiment.value("code_rangee").toString());
                            }
                        }
                        else
                        {
                            qDebug() << "erreur requeteCompartiment";
                        }


                        QString s_requeteJeu = "SELECT url_photo FROM photo WHERE fk_id_jouet = " + requeteStock.value("fk_id_jouet").toString() + ";";
                        QSqlQuery requeteJeu;

                        if(requeteJeu.exec(s_requeteJeu))
                        {
                            while(requeteJeu.next())
                            {
                                qDebug() << requeteJeu.value("url_photo").toString();
                                // download la photo + la mettre dans le label pour la photo
                                QString s_chemin = qApp->applicationDirPath() + "/images/tmp_dl.jpg";
                                downloadImage(requeteJeu.value("url_photo").toString(), s_chemin);
                                QPixmap pixPhoto(s_chemin);
                                page_resultatChercherLabelPhoto->setPixmap(pixPhoto.scaled(600,400,Qt::KeepAspectRatio));
                            }
                        }
                        else
                        {
                            qDebug() << "erreur requeteJeu";
                        }

                        stackedWidget->setCurrentWidget(page_resultatChercher);
                    }
                }
            }
            else
            {
                qDebug() << "erreur requeteStock";
            }
        }
        else
        {
            qDebug() << "erreur connexion bdd";
        }
    });
    connect(page_chercherDialogButtonBox, &QDialogButtonBox::rejected, this, [&]() {
        stackedWidget->setCurrentWidget(page_choix);
    });
    page_chercherLayout->addWidget(page_chercherDialogButtonBox);

    /*
     *
     * page_ranger
     *
     * */
    page_rangerLayout = new QVBoxLayout;
    page_ranger->setLayout(page_rangerLayout);

    page_rangerGroupBox = new QGroupBox();
    page_rangerGroupBox->setTitle("Ranger un article :");
    page_rangerGroupBoxLayout = new QFormLayout;
    page_rangerLineEditCodeBarresIdentification = new QLineEdit;
    // en cas d'erreur permet à l'utilisateur de voir le champ avec l'erreur
    //page_rangerLineEditCodeBarresIdentification->setStyleSheet("border: 1px solid red");
    page_rangerLineEditCodeBarresCompartiment = new QLineEdit;

    page_rangerGroupBoxLayout->addRow("Code-barres identification :", page_rangerLineEditCodeBarresIdentification);
    page_rangerGroupBoxLayout->addRow("Code-barres compartiment :", page_rangerLineEditCodeBarresCompartiment);
    page_rangerGroupBox->setLayout(page_rangerGroupBoxLayout);
    page_rangerLayout->addWidget(page_rangerGroupBox);

    page_rangerDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(page_rangerDialogButtonBox, &QDialogButtonBox::accepted, this, [&]() {
        // Etape 1 : on vérifie que le code-barres compartiment existe
        if(db.open())
        {
            QString s_requeteCodeBarres = "SELECT * FROM exemplaire WHERE code_barres_exemplaire = '" + page_rangerLineEditCodeBarresIdentification->text() +"';";
            QSqlQuery requeteCodeBarres;
            if(requeteCodeBarres.exec(s_requeteCodeBarres))
            {
                // Vérifie si le code-barres d'identification est présent dans la table compartiment
                if(requeteCodeBarres.size() == 0)
                {
                    QMessageBox::information(this, "Erreur code-barres identification", "Aucun code-barres d'idendification correspondant trouvé dans la bdd !");
                }
                else
                {
                    QString s_requeteCompartiment = "SELECT * FROM compartiment WHERE code_barres_compartiment = '" + page_rangerLineEditCodeBarresCompartiment->text() +"';";
                    QSqlQuery requeteCompartiment;
                    if(requeteCompartiment.exec(s_requeteCompartiment))
                    {
                        // Vérifie si le code-barres compartiment est présent dans la table compartiment
                        if(requeteCompartiment.size() == 0)
                        {
                            QMessageBox::information(this, "Erreur code-barres compartiment", "Aucun code-barres compartiment correspondant trouvé dans la bdd !");
                        }
                        else
                        {
                            // si tout est ok on ajoute dans la bdd
                            QSqlQuery query;
                            QString s_query = "UPDATE exemplaire SET fk_code_barres_compartiment = " + page_rangerLineEditCodeBarresCompartiment->text() + ";";
                            query.exec(s_query);
                            QMessageBox::information(this, "Information", "Article ajouté dans le stock !");
                            // réinitialisation des champs
                            page_rangerLineEditCodeBarresIdentification->setText("");
                        }
                    }
                    else
                    {
                        qDebug() << "erreur requeteCompartiment";
                    }
                }
            }
            else
            {
                qDebug() << "erreur requeteCodeBarres";
            }
        }
        else
        {
            qDebug() << "erreur connexion bdd";
        }
    });
    connect(page_rangerDialogButtonBox, &QDialogButtonBox::rejected, this, [&]() {
        stackedWidget->setCurrentWidget(page_choix);
    });
    page_rangerLayout->addWidget(page_rangerDialogButtonBox);

    // page_resultatChercher
    page_resultatChercherLayout = new QVBoxLayout;
    page_resultatChercher->setLayout(page_resultatChercherLayout);

    page_resultatChercherGroupBox = new QGroupBox;
    page_resultatChercherGroupBox->setTitle("Emplacement de l'article :");
    page_resultatChercherGroupBoxLayout = new QFormLayout;
    page_resultatChercherLabelPhoto = new QLabel;
    page_resultatChercherLabelCodeRangee = new QLabel;
    page_resultatChercherLabelCodeEtage = new QLabel;
    page_resultatChercherLabelCodeIdentification = new QLabel;
    page_resultatChercherLineEditCodeBarresIdentification = new QLineEdit;

    page_resultatChercherGroupBoxLayout->addRow("Photo :", page_resultatChercherLabelPhoto);
    page_resultatChercherGroupBoxLayout->addRow("Code rangée :", page_resultatChercherLabelCodeRangee);
    page_resultatChercherGroupBoxLayout->addRow("Code étage :", page_resultatChercherLabelCodeEtage);
    page_resultatChercherGroupBoxLayout->addRow("Code identification :", page_resultatChercherLabelCodeIdentification);
    page_resultatChercherGroupBoxLayout->addRow("Code-barres identification :", page_resultatChercherLineEditCodeBarresIdentification);
    page_resultatChercherGroupBox->setLayout(page_resultatChercherGroupBoxLayout);
    page_resultatChercherLayout->addWidget(page_resultatChercherGroupBox);

    page_resultatChercherDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    page_resultatChercherLayout->addWidget(page_resultatChercherDialogButtonBox);
}

Widget::~Widget()
{

}
