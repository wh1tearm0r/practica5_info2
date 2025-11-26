#ifndef PARTICULA_H
#define PARTICULA_H

#include <cmath>

class Particula {
private:
    double x, y;
    double vx, vy;
    double masa;
    double radio;
    bool activa;
    int id;

public:
    Particula(double x0, double y0, double vx0, double vy0, double m, int identificador);
    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getMasa() const { return masa; }
    double getRadio() const { return radio; }
    bool estaActiva() const { return activa; }
    int getId() const { return id; }

    void setX(double newX) { x = newX; }
    void setY(double newY) { y = newY; }
    void setVx(double v) { vx = v; }
    void setVy(double v) { vy = v; }
    void setMasa(double m) { masa = m; }
    void setRadio(double r) { radio = r; }
    void desactivar() { activa = false; }
    void actualizarPosicion(double dt);
    double distanciaA(const Particula& otra) const;
    bool colisionaCon(const Particula& otra) const;
    void fusionarCon(Particula& otra);
    void rebotarParedVertical();
    void rebotarParedHorizontal();
};

#endif // PARTICULA_H
