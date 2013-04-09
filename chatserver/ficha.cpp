#include "ficha.h"

Ficha::Ficha()
{
    this->bando=-1;
    this->posX=-1;
    this->posY=-1;
    this->tipo="";
}
Ficha::Ficha(int ban, int posX, int posY,QString tipo){
    this->bando=ban;
    this->posX=posX;
    this->posY=posY;
    this->tipo=tipo;
}
