#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "simulador.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void iniciarSimulacion();
    void pausarSimulacion();
    void resetearSimulacion();
    void actualizarSimulacion();
    void exportarDatos();

private:
    Ui::MainWindow *ui;

    // Simulador
    Simulador *simulador;

    // Graficos
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::vector<QGraphicsEllipseItem*> itemsParticulas;
    std::vector<QGraphicsRectItem*> itemsObstaculos;
    QGraphicsRectItem *itemCaja;

    // Timer para animacion
    QTimer *timer;

    // Elementos de texto
    QGraphicsTextItem *textoEstadisticas;

    // Control
    bool simulacionActiva;
    int pasoActual;

    // Botones
    QPushButton *btnIniciar;
    QPushButton *btnPausar;
    QPushButton *btnReset;
    QPushButton *btnExportar;

    // Metodos auxiliares
    void configurarInterfaz();
    void configurarEscena();
    void dibujarObstaculos();
    void crearParticulas();
    void actualizarVisualizacion();
    void actualizarEstadisticas();
    void limpiarEscena();
};

#endif // MAINWINDOW_H
