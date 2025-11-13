#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <QPen>
#include <QBrush>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , simulador(nullptr)
    , scene(nullptr)
    , view(nullptr)
    , timer(nullptr)
    , textoEstadisticas(nullptr)
    , itemCaja(nullptr)
    , simulacionActiva(false)
    , pasoActual(0)
    , btnIniciar(nullptr)
    , btnPausar(nullptr)
    , btnReset(nullptr)
    , btnExportar(nullptr)
{
    ui->setupUi(this);

    // Configurar ventana
    setWindowTitle("Simulador de Colisiones - Práctica 5 - Informática II");
    resize(1000, 750);

    // Configurar interfaz
    configurarInterfaz();

    // Configurar escena inicial
    configurarEscena();
}

MainWindow::~MainWindow()
{
    // Detener timer primero
    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    // Limpiar escena
    limpiarEscena();

    // Eliminar scene
    if (scene) {
        delete scene;
        scene = nullptr;
    }

    // Eliminar simulador
    if (simulador) {
        delete simulador;
        simulador = nullptr;
    }

    // UI se elimina automaticamente
    delete ui;
}

void MainWindow::configurarInterfaz()
{
    // Crear widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(centralWidget);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);

    // Crear scene y view
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    scene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(820, 620);
    view->setMaximumSize(820, 620);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layoutPrincipal->addWidget(view);

    // Panel de control
    QHBoxLayout *layoutControles = new QHBoxLayout();

    btnIniciar = new QPushButton(" Iniciar", this);
    btnPausar = new QPushButton(" Pausar", this);
    btnReset = new QPushButton(" Resetear", this);
    btnExportar = new QPushButton(" Exportar", this);

    btnIniciar->setMinimumHeight(40);
    btnPausar->setMinimumHeight(40);
    btnReset->setMinimumHeight(40);
    btnExportar->setMinimumHeight(40);

    // Estilo de botones
    QString estiloBase = "QPushButton { font-size: 14px; font-weight: bold; padding: 8px; border-radius: 5px; border: 2px solid; }";
    btnIniciar->setStyleSheet(estiloBase + "QPushButton { background-color: #4CAF50; color: white; border-color: #45a049; }");
    btnPausar->setStyleSheet(estiloBase + "QPushButton { background-color: #FF9800; color: white; border-color: #e68900; }");
    btnReset->setStyleSheet(estiloBase + "QPushButton { background-color: #2196F3; color: white; border-color: #0b7dda; }");
    btnExportar->setStyleSheet(estiloBase + "QPushButton { background-color: #9C27B0; color: white; border-color: #7b1fa2; }");

    layoutControles->addWidget(btnIniciar);
    layoutControles->addWidget(btnPausar);
    layoutControles->addWidget(btnReset);
    layoutControles->addWidget(btnExportar);
    layoutControles->addStretch();

    layoutPrincipal->addLayout(layoutControles);

    // Conectar seniales
    connect(btnIniciar, &QPushButton::clicked, this, &MainWindow::iniciarSimulacion);
    connect(btnPausar, &QPushButton::clicked, this, &MainWindow::pausarSimulacion);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::resetearSimulacion);
    connect(btnExportar, &QPushButton::clicked, this, &MainWindow::exportarDatos);

    // Timer para animacion
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarSimulacion);
}

void MainWindow::configurarEscena()
{
    if (!scene) return;

    // Limpiar primero
    limpiarEscena();

    // Crear simulador
    if (simulador) {
        delete simulador;
    }
    simulador = new Simulador(800, 600, 0.01);

    // Dibujar limite de la caja
    itemCaja = scene->addRect(0, 0, 800, 600, QPen(Qt::black, 3), Qt::NoBrush);

    // Agregar 4 obstaculos
    simulador->agregarObstaculo(200, 150, 80, 80, 0.7);
    simulador->agregarObstaculo(520, 150, 80, 80, 0.7);
    simulador->agregarObstaculo(200, 370, 80, 80, 0.7);
    simulador->agregarObstaculo(520, 370, 80, 80, 0.7);

    // Agregar 4 particulas
    simulador->agregarParticula(100, 100, 150, 120, 1.0);
    simulador->agregarParticula(700, 100, -180, 140, 1.5);
    simulador->agregarParticula(100, 500, 160, -130, 1.2);
    simulador->agregarParticula(700, 500, -170, -110, 0.8);

    // Dibujar elementos
    dibujarObstaculos();
    crearParticulas();

    // Texto de estadisticas
    textoEstadisticas = scene->addText("");
    QFont fuente("Arial", 10, QFont::Bold);
    textoEstadisticas->setFont(fuente);
    textoEstadisticas->setPos(10, 10);
    textoEstadisticas->setDefaultTextColor(Qt::black);
    textoEstadisticas->setZValue(100); // Encima de todo

    actualizarEstadisticas();
    pasoActual = 0;
}

void MainWindow::dibujarObstaculos()
{
    if (!scene || !simulador) return;

    // Limpiar obstaculos anteriores
    for (auto item : itemsObstaculos) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsObstaculos.clear();

    // Dibujar nuevos obstaculos
    QColor colorObstaculo(100, 100, 100, 180);
    QPen penObstaculo(Qt::black, 2);

    const auto& obstaculos = simulador->getObstaculos();
    for (size_t i = 0; i < obstaculos.size(); i++) {
        const auto& obs = obstaculos[i];
        if (obs) {
            QGraphicsRectItem *rect = scene->addRect(
                obs->getX(),
                obs->getY(),
                obs->getAncho(),
                obs->getAlto(),
                penObstaculo,
                QBrush(colorObstaculo)
                );
            itemsObstaculos.push_back(rect);
        }
    }
}

void MainWindow::crearParticulas()
{
    if (!scene || !simulador) return;

    // Limpiar particulas anteriores
    for (auto item : itemsParticulas) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsParticulas.clear();

    // Colores para cada particula
    QList<QColor> colores = {
        QColor(255, 50, 50),   // Rojo
        QColor(50, 50, 255),   // Azul
        QColor(50, 255, 50),   // Verde
        QColor(255, 165, 0)    // Naranja
    };

    // Crear circulos para cada particula
    const auto& particulas = simulador->getParticulas();
    for (size_t i = 0; i < particulas.size(); i++) {
        QColor color = colores[i % colores.size()];
        QPen pen(Qt::black, 2);

        QGraphicsEllipseItem *item = scene->addEllipse(
            0, 0, 10, 10,  // Valores iniciales (se actualizaran)
            pen,
            QBrush(color)
            );

        itemsParticulas.push_back(item);
    }

    // Actualizar posiciones iniciales
    actualizarVisualizacion();
}

void MainWindow::actualizarVisualizacion()
{
    if (!simulador) return;

    const auto& particulas = simulador->getParticulas();

    for (size_t i = 0; i < particulas.size() && i < itemsParticulas.size(); i++) {
        if (particulas[i] && itemsParticulas[i]) {
            if (particulas[i]->estaActiva()) {
                double x = particulas[i]->getX();
                double y = particulas[i]->getY();
                double radio = particulas[i]->getRadio();

                // Actualizar posicion y tamanio
                itemsParticulas[i]->setRect(
                    x - radio,
                    y - radio,
                    radio * 2,
                    radio * 2
                    );
                itemsParticulas[i]->setVisible(true);
            } else {
                // Ocultar particulas inactivas
                itemsParticulas[i]->setVisible(false);
            }
        }
    }
}

void MainWindow::actualizarEstadisticas()
{
    if (!textoEstadisticas || !simulador) return;

    QString stats = QString(
                        " Tiempo: %1 s |  Activas: %2/%3 |  Colisiones: %4 |  Pasos: %5"
                        ).arg(simulador->getTiempoActual(), 0, 'f', 2)
                        .arg(simulador->getNumParticulasActivas())
                        .arg(simulador->getParticulas().size())
                        .arg(simulador->getEventosColision().size())
                        .arg(pasoActual);

    textoEstadisticas->setPlainText(stats);
}

void MainWindow::iniciarSimulacion()
{
    if (!simulacionActiva && timer) {
        simulacionActiva = true;
        timer->start(16);  // ~60 FPS
        if (ui && ui->statusbar) {
            ui->statusbar->showMessage(" Simulacion en ejecucion...", 2000);
        }
    }
}

void MainWindow::pausarSimulacion()
{
    if (simulacionActiva && timer) {
        simulacionActiva = false;
        timer->stop();
        if (ui && ui->statusbar) {
            ui->statusbar->showMessage(" Simulacion pausada", 2000);
        }
    }
}

void MainWindow::resetearSimulacion()
{
    pausarSimulacion();
    configurarEscena();
    if (ui && ui->statusbar) {
        ui->statusbar->showMessage(" Simulacion reseteada", 2000);
    }
}

void MainWindow::actualizarSimulacion()
{
    if (!simulador) return;

    // Ejecutar paso
    simulador->simularPaso();
    pasoActual++;

    // Actualizar visualizacion
    actualizarVisualizacion();
    actualizarEstadisticas();
}

void MainWindow::exportarDatos()
{
    if (!simulador) {
        QMessageBox::warning(this, "Error", "No hay simulacion para exportar");
        return;
    }

    QString nombreArchivo = QFileDialog::getSaveFileName(
        this,
        "Guardar resultados",
        "simulacion.txt",
        "Archivos de texto (*.txt)"
        );

    if (nombreArchivo.isEmpty()) {
        return;
    }

    // Exportar
    simulador->exportarSimulacion(nombreArchivo.toStdString());

    QMessageBox::information(
        this,
        " Exportacion exitosa",
        "Datos guardados en:\n" + nombreArchivo
        );

    if (ui && ui->statusbar) {
        ui->statusbar->showMessage(" Datos exportados", 3000);
    }
}

void MainWindow::limpiarEscena()
{
    if (!scene) return;

    // Limpiar partículas
    for (auto item : itemsParticulas) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsParticulas.clear();

    // Limpiar obstaculos
    for (auto item : itemsObstaculos) {
        if (item) {
            scene->removeItem(item);
            delete item;
        }
    }
    itemsObstaculos.clear();

    // Limpiar caja
    if (itemCaja) {
        scene->removeItem(itemCaja);
        delete itemCaja;
        itemCaja = nullptr;
    }

    // Limpiar texto
    if (textoEstadisticas) {
        scene->removeItem(textoEstadisticas);
        delete textoEstadisticas;
        textoEstadisticas = nullptr;
    }
}
