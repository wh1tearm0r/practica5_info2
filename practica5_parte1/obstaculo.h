#ifndef OBSTACULO_H
#define OBSTACULO_H

class Particula;

class Obstaculo {
private:
    double x, y;
    double ancho, alto;
    double coefRestitucion;

public:
    Obstaculo(double x0, double y0, double w, double h, double e = 0.7);
    double getX() const { return x; }
    double getY() const { return y; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    double getCoefRestitucion() const { return coefRestitucion; }
    bool colisionaCon(const Particula& p) const;
    bool aplicarRebote(Particula& p) const;

private:
    enum Lado { NINGUNO, IZQUIERDO, DERECHO, SUPERIOR, INFERIOR };
    Lado determinarLadoColision(const Particula& p) const;
};

#endif // OBSTACULO_H
