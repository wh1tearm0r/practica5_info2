#ifndef MOTORCOLISIONES_H
#define MOTORCOLISIONES_H

#include "Proyectil.h"
#include "Muro.h"
#include <QRectF>
#include <QPointF>
#include <vector>
#include <QString>

struct InfoColision {
    bool ocurrioColision;
    QString tipoColision;
    double momentoImpacto;
    int indiceObjetoImpactado;
    QPointF puntoImpacto;

    InfoColision()
        : ocurrioColision(false),
        tipoColision(""),
        momentoImpacto(0.0),
        indiceObjetoImpactado(-1),
        puntoImpacto(0, 0) {}
};
class MotorColisiones {
private:
    QRectF limitesEscenario;
    double coefRestitucionMuros;
    bool puntoEnRectangulo(const QPointF& punto, const QRectF& rect, double margen) const;

public:
    MotorColisiones(double ancho, double alto, double coefMuros = 0.7);
    InfoColision verificarColisiones(Proyectil& proyectil,
                                     const std::vector<Muro*>& muros,
                                     const QPointF& posRival1,
                                     const QPointF& posRival2);
    bool procesarColisionParedes(Proyectil& proyectil);
    bool procesarColisionMuro(Proyectil& proyectil, Muro& muro);
    bool verificarImpactoRival(const Proyectil& proyectil,
                               const QPointF& posRival,
                               double radioDeteccion = 15.0) const;
    double calcularMomentoLineal(const Proyectil& proyectil) const;
    QRectF getLimitesEscenario() const { return limitesEscenario; }
    void setLimitesEscenario(const QRectF& limites) { limitesEscenario = limites; }
    double getCoefRestitucionMuros() const { return coefRestitucionMuros; }
};

#endif // MOTORCOLISIONES_H
