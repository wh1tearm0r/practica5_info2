#include "Proyectil.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Proyectil::Proyectil(double x0, double y0, double angulo, double v0,
                     double masa, int jugador, double g)
    : Particula(x0, y0, 0, 0, masa, -1),
    anguloLanzamiento(angulo),
    velocidadInicial(v0),
    tiempoVuelo(0.0),
    idJugadorDueno(jugador),
    gravedad(g)
{
    double anguloRad = angulo * M_PI / 180.0;
    double vx0 = v0 * cos(anguloRad);
    double vy0 = v0 * sin(anguloRad);
    setVx(vx0);
    setVy(-vy0);
}

void Proyectil::actualizarMovimiento(double dt) {
    if (!estaActiva()) return;

    tiempoVuelo += dt;

    double vx = getVx();
    double vy = getVy() + gravedad * dt;

    setVy(vy);
    actualizarPosicion(dt);
}

double Proyectil::getVelocidadActual() const {
    double vx = getVx();
    double vy = getVy();
    return sqrt(vx * vx + vy * vy);
}

double Proyectil::getMomentoLineal() const {
    return getMasa() * getVelocidadActual();
}

void Proyectil::rebotarParedElastico() {
}
