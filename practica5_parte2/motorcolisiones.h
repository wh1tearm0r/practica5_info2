#ifndef MOTORCOLISIONES_H
#define MOTORCOLISIONES_H

#include <QPointF>
#include <QRectF>
#include <vector>
#include <string>
#include "proyectil.h"
#include "muro.h"

struct InfoColision {
    bool ocurrioColision;
    std::string tipoColision;
    double momentoImpacto;
    int indiceObjetoImpactado;
    QPointF puntoImpacto;
};

class MotorColisiones {
private:
    double anchoEscenario;
    double altoEscenario;
    double coefRestitucionParedes;

public:
    MotorColisiones(double ancho, double alto, double coefPared = 1.0);

    InfoColision verificarColisiones(Proyectil &proyectil,
                                     std::vector<Muro*> &muros,
                                     const QPointF &rivalJ1,
                                     const QPointF &rivalJ2,
                                     int jugadorAtacante);
};

#endif // MOTORCOLISIONES_H
