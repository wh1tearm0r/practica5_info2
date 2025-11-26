#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QObject>
#include <QGraphicsRectItem>

class personaje : public QObject, public QGraphicsRectItem
{
protected:
    int vida;
    int ataque;

    /* La infraestructura de tanto el jugador como el enemigo se heredarán de
     * esta clase */

public:
    personaje();
};

#endif // PERSONAJE_H
