#ifndef MURO_H
#define MURO_H

#include "Obstaculo.h"

class Muro : public Obstaculo {

private:
    double resistencia;
    double resistenciaMaxima;
    int idJugadorPropietario;

public:
    Muro(double x, double y, double ancho, double alto,
         double resistenciaInicial, int propietario,
         double coefRestitucion = 0.7);
    void recibirDanio(double danio);
    bool estaDestruido() const { return resistencia <= 0.0; }
    double getPorcentajeVida() const;
    double getResistencia() const { return resistencia; }
    double getResistenciaMaxima() const { return resistenciaMaxima; }
    int getIdPropietario() const { return idJugadorPropietario; }
    void restaurar();
};

#endif // MURO_H
