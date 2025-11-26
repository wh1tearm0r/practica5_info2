#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "Particula.h"
#include <cmath>

class Proyectil : public Particula {
private:
    double anguloLanzamiento;
    double velocidadInicial;
    double tiempoVuelo;
    int idJugadorDueno;
    double gravedad;

public:
    Proyectil(double x0, double y0, double angulo, double v0,
              double masa, int jugador, double g = 980.0);
    void actualizarMovimiento(double dt);
    double getVelocidadActual() const;
    double getMomentoLineal() const;
    double getAnguloLanzamiento() const { return anguloLanzamiento; }
    double getVelocidadInicial() const { return velocidadInicial; }
    double getTiempoVuelo() const { return tiempoVuelo; }
    int getIdJugador() const { return idJugadorDueno; }
    double getGravedad() const { return gravedad; }

    void rebotarParedElastico();
};

#endif // PROYECTIL_H
