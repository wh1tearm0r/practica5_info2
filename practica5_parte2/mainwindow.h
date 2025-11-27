#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QTimer>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QKeyEvent>
#include <vector>
#include "proyectil.h"
#include "muro.h"
#include "motorcolisiones.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void disparar();
    void resetearJuego();
    void actualizarInfoAngulo(int valor);
    void actualizarInfoVelocidad(int valor);
    void actualizarSimulacion();

private:
    Ui::MainWindow *ui;

    // Escena y Vista
    QGraphicsScene *scene;
    QGraphicsView *view;

    // Items graficos
    QGraphicsRectItem *itemSuelo;
    QGraphicsRectItem *itemCanonJ1;
    QGraphicsRectItem *itemCanonJ2;
    QGraphicsLineItem *tuboCanonJ1;
    QGraphicsLineItem *tuboCanonJ2;
    QGraphicsEllipseItem *itemRivalJ1;
    QGraphicsEllipseItem *itemRivalJ2;
    QGraphicsEllipseItem *itemProyectil;

    // Items de muros
    std::vector<QGraphicsRectItem*> itemsMuros;
    std::vector<QGraphicsTextItem*> textosVidaMuros;

    // Textos informativos
    QGraphicsTextItem *textoTurno;
    QGraphicsTextItem *textoEstado;
    QGraphicsTextItem *textoInstrucciones;

    // Controles UI
    QSlider *sliderAngulo;
    QSlider *sliderVelocidad;
    QSpinBox *spinAngulo;
    QSpinBox *spinVelocidad;
    QPushButton *btnDisparar;
    QPushButton *btnReset;
    QLabel *labelTurno;
    QLabel *labelAngulo;
    QLabel *labelVelocidad;

    // Motor del juego
    QTimer *timer;
    Proyectil *proyectilActivo;
    std::vector<Muro*> muros;
    MotorColisiones *motorColisiones;

    // Estado del juego
    int turnoActual;
    bool juegoActivo;
    bool proyectilEnMovimiento;

    // Constantes
    const double ANCHO_ESCENARIO = 1000.0;
    const double ALTO_ESCENARIO = 600.0;
    const double MASA_PROYECTIL = 1.0;
    const double GRAVEDAD = 980.0;
    const double DT = 0.016;
    const double FACTOR_DANIO = 0.05;

    // Posiciones
    QPointF posCanonJ1;
    QPointF posCanonJ2;
    QPointF posRivalJ1;
    QPointF posRivalJ2;

    // Metodos privados
    void configurarInterfaz();
    void configurarEscena();
    void crearElementosEstaticos();
    void crearMuros();
    void crearControles();
    void dibujarCanon(QGraphicsRectItem* &baseCanon, QGraphicsLineItem* &tubo,
                      const QPointF &pos, int jugador);
    void dibujarRival(QGraphicsEllipseItem* &rival, const QPointF &pos, int jugador);
    void actualizarVisualizacion();
    void actualizarBarrasVida();
    void limpiarProyectil();
    QColor obtenerColorJugador(int jugador);
    void cambiarTurno();
    void verificarColisiones();
    void mostrarVictoria(int jugadorGanador);
    void actualizarAnguloCanon();
};

#endif // MAINWINDOW_H
