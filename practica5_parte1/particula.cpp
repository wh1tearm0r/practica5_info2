#include "Particula.h"

Particula::Particula(double x0, double y0, double vx0, double vy0, double m, int identificador)
    : x(x0), y(y0), vx(vx0), vy(vy0), masa(m), activa(true), id(identificador) {
    radio = 5.0 * sqrt(masa);
}

void Particula::actualizarPosicion(double dt) {
    if (!activa) return;

    x += vx * dt;
    y += vy * dt;
}

double Particula::distanciaA(const Particula& otra) const {
    double dx = x - otra.x;
    double dy = y - otra.y;
    return sqrt(dx*dx + dy*dy);
}

bool Particula::colisionaCon(const Particula& otra) const {
    if (!activa || !otra.activa) return false;
    double distancia = distanciaA(otra);
    return distancia <= (radio + otra.radio);
}

void Particula::fusionarCon(Particula& otra) {
    if (!activa || !otra.activa) return;
    double masaTotal = masa + otra.masa;

    double nuevaVx = (masa * vx + otra.masa * otra.vx) / masaTotal;
    double nuevaVy = (masa * vy + otra.masa * otra.vy) / masaTotal;

    double nuevaX = (masa * x + otra.masa * otra.x) / masaTotal;
    double nuevaY = (masa * y + otra.masa * otra.y) / masaTotal;

    x = nuevaX;
    y = nuevaY;
    vx = nuevaVx;
    vy = nuevaVy;
    masa = masaTotal;
    radio = 5.0 * sqrt(masa);

    otra.desactivar();
}

void Particula::rebotarParedVertical() {
    vx = -vx;
}
void Particula::rebotarParedHorizontal() {
    vy = -vy;
}
