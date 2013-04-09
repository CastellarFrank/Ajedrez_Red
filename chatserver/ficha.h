#ifndef FICHA_H
#define FICHA_H
#include <QString>
class Ficha
{
public:
    Ficha();
    Ficha(int ban,int posX,int posY,QString tipo);
    int posX;
    int posY;
    int bando;
    QString tipo;
};

#endif // FICHA_H
