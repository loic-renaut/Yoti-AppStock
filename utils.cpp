#include <stdio.h>
#include <curl/curl.h>
#include <QDebug>
#include <QDir>

using namespace std;

void newDir(QString dirPath)
{
    QDir dir(dirPath);
    QDir dir2;
    if(!dir.exists())
    {
        qDebug() << "création du répertoire " << dirPath;
        dir2.mkpath(dirPath);
    }
    else
    {
        qDebug() << "erreur création du répertoire " << dirPath;
    }
}

/*
 *
 * Fonction qui permet de télécharger une image à partir d'une url donnée en premier paramètre. L'image est sauvegardée à l'emplacement donné en second paramètre.
 *
 * */
void downloadImage(QString url_image, QString chemin_image)
{
    string url = url_image.toUtf8().constData();
    string fileName = chemin_image.toStdString();
    char* char_fileName;
    char_fileName = &fileName[0];
    char* char_url;
    char_url = &url[0];
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();

    if (curl)
    {
        fp = fopen(char_fileName,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, char_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        const char *str = curl_easy_strerror(res);
        qDebug() << "libcurl said" << str << endl;
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}
