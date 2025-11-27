#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , view(nullptr)
    , itemEscenario(nullptr)
    , itemSuelo(nullptr)
    , itemCanonJ1(nullptr)
    , itemCanonJ2(nullptr)
    , tuboCanonJ1(nullptr)
    , tuboCanonJ2(nullptr)
    , itemRivalJ1(nullptr)
    , itemRivalJ2(nullptr)
    , itemProyectil(nullptr)
    , textoTurno(nullptr)
    , textoEstado(nullptr)
    , timer(nullptr)
    , proyectilActivo(nullptr)
    , motorColisiones(nullptr)
    , turnoActual(1)
    , juegoActivo(true)
    , proyectilEnMovimiento(false)
{
    ui->setupUi(this);

    // Configuracion de la ventana
    setWindowTitle("Juego de Estrategia Militar - Practica 5 - Informatica II");
    resize(1400, 800);

    // Definir posiciones de elementos
    posCanonJ1 = QPointF(50, ALTO_ESCENARIO - 50);
    posCanonJ2 = QPointF(ANCHO_ESCENARIO - 50, ALTO_ESCENARIO - 50);
    posRivalJ1 = QPointF(150, ALTO_ESCENARIO - 100);
    posRivalJ2 = QPointF(ANCHO_ESCENARIO - 150, ALTO_ESCENARIO - 100);

    // Configurar interfaz y escena
    configurarInterfaz();
    configurarEscena();

    // Timer para animacion
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarSimulacion);
}

MainWindow::~MainWindow()
{
    if (timer) {
        timer->stop();
        delete timer;
    }

    limpiarProyectil();

    // Limpiar muros
    for (auto muro : muros) {
        delete muro;
    }
    muros.clear();

    if (motorColisiones) {
        delete motorColisiones;
    }

    if (scene) {
        delete scene;
    }

    delete ui;
}

// ==== CONFIGURACION DE LA INTERFAZ GRAFICA ====

void MainWindow::configurarInterfaz()
{
    // Widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal horizontal (Escena + Panel de controles)
    QHBoxLayout *layoutPrincipal = new QHBoxLayout(centralWidget);
    layoutPrincipal->setSpacing(15);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);

    // ===== PANEL IZQUIERDO: ESCENA DE JUEGO =====
    QVBoxLayout *layoutIzquierdo = new QVBoxLayout();

    // Crear escena y vista
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, ANCHO_ESCENARIO, ALTO_ESCENARIO);
    scene->setBackgroundBrush(QBrush(QColor(135, 206, 235))); // Cielo azul

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFixedSize(static_cast<int>(ANCHO_ESCENARIO) + 20, static_cast<int>(ALTO_ESCENARIO) + 20);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setStyleSheet("border: 3px solid #333333; background-color: #f0f0f0;");

    layoutIzquierdo->addWidget(view);
    layoutPrincipal->addLayout(layoutIzquierdo);

    // ===== PANEL DERECHO: CONTROLES =====
    crearControles();
}

void MainWindow::crearControles()
{
    QVBoxLayout *layoutDerecho = new QVBoxLayout();
    layoutDerecho->setSpacing(15);

    // ===== GRUPO: TURNO ACTUAL =====
    QGroupBox *grupoTurno = new QGroupBox("TURNO ACTUAL");
    grupoTurno->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; }");
    QVBoxLayout *layoutTurno = new QVBoxLayout();

    labelTurno = new QLabel("JUGADOR 1");
    QFont fuenteTurno("Arial", 22, QFont::Bold);
    labelTurno->setFont(fuenteTurno);
    labelTurno->setAlignment(Qt::AlignCenter);
    labelTurno->setStyleSheet(
        "color: #FF4444; "
        "background-color: #FFE6E6; "
        "padding: 20px; "
        "border-radius: 10px; "
        "border: 3px solid #FF4444;"
        );

    layoutTurno->addWidget(labelTurno);
    grupoTurno->setLayout(layoutTurno);
    layoutDerecho->addWidget(grupoTurno);

    // ===== GRUPO: CONTROL DE ANGULO =====
    QGroupBox *grupoAngulo = new QGroupBox("ANGULO DE DISPARO");
    grupoAngulo->setStyleSheet("QGroupBox { font-weight: bold; font-size: 12px; }");
    QVBoxLayout *layoutAngulo = new QVBoxLayout();

    labelAngulo = new QLabel("Angulo: 45°");
    labelAngulo->setAlignment(Qt::AlignCenter);
    QFont fuenteLabel("Arial", 12, QFont::Bold);
    labelAngulo->setFont(fuenteLabel);
    labelAngulo->setStyleSheet("padding: 5px; background-color: #E8F5E9; border-radius: 5px;");

    sliderAngulo = new QSlider(Qt::Horizontal);
    sliderAngulo->setMinimum(0);
    sliderAngulo->setMaximum(90);
    sliderAngulo->setValue(45);
    sliderAngulo->setTickPosition(QSlider::TicksBelow);
    sliderAngulo->setTickInterval(15);
    sliderAngulo->setStyleSheet(
        "QSlider::groove:horizontal { height: 8px; background: #CCCCCC; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: #4CAF50; width: 18px; margin: -5px 0; border-radius: 9px; }"
        );

    spinAngulo = new QSpinBox();
    spinAngulo->setMinimum(0);
    spinAngulo->setMaximum(90);
    spinAngulo->setValue(45);
    spinAngulo->setSuffix("°");
    spinAngulo->setAlignment(Qt::AlignCenter);
    spinAngulo->setStyleSheet("font-size: 14px; padding: 5px;");

    connect(sliderAngulo, &QSlider::valueChanged, this, &MainWindow::actualizarInfoAngulo);
    connect(sliderAngulo, &QSlider::valueChanged, spinAngulo, &QSpinBox::setValue);
    connect(spinAngulo, QOverload<int>::of(&QSpinBox::valueChanged), sliderAngulo, &QSlider::setValue);

    layoutAngulo->addWidget(labelAngulo);
    layoutAngulo->addWidget(sliderAngulo);
    layoutAngulo->addWidget(spinAngulo);
    grupoAngulo->setLayout(layoutAngulo);
    layoutDerecho->addWidget(grupoAngulo);

    // ===== GRUPO: CONTROL DE VELOCIDAD =====
    QGroupBox *grupoVelocidad = new QGroupBox("VELOCIDAD INICIAL");
    grupoVelocidad->setStyleSheet("QGroupBox { font-weight: bold; font-size: 12px; }");
    QVBoxLayout *layoutVelocidad = new QVBoxLayout();

    labelVelocidad = new QLabel("Velocidad: 500 px/s");
    labelVelocidad->setAlignment(Qt::AlignCenter);
    labelVelocidad->setFont(fuenteLabel);
    labelVelocidad->setStyleSheet("padding: 5px; background-color: #E3F2FD; border-radius: 5px;");

    sliderVelocidad = new QSlider(Qt::Horizontal);
    sliderVelocidad->setMinimum(200);
    sliderVelocidad->setMaximum(800);
    sliderVelocidad->setValue(500);
    sliderVelocidad->setTickPosition(QSlider::TicksBelow);
    sliderVelocidad->setTickInterval(100);
    sliderVelocidad->setStyleSheet(
        "QSlider::groove:horizontal { height: 8px; background: #CCCCCC; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: #2196F3; width: 18px; margin: -5px 0; border-radius: 9px; }"
        );

    spinVelocidad = new QSpinBox();
    spinVelocidad->setMinimum(200);
    spinVelocidad->setMaximum(800);
    spinVelocidad->setValue(500);
    spinVelocidad->setSuffix(" px/s");
    spinVelocidad->setAlignment(Qt::AlignCenter);
    spinVelocidad->setStyleSheet("font-size: 14px; padding: 5px;");

    connect(sliderVelocidad, &QSlider::valueChanged, this, &MainWindow::actualizarInfoVelocidad);
    connect(sliderVelocidad, &QSlider::valueChanged, spinVelocidad, &QSpinBox::setValue);
    connect(spinVelocidad, QOverload<int>::of(&QSpinBox::valueChanged), sliderVelocidad, &QSlider::setValue);

    layoutVelocidad->addWidget(labelVelocidad);
    layoutVelocidad->addWidget(sliderVelocidad);
    layoutVelocidad->addWidget(spinVelocidad);
    grupoVelocidad->setLayout(layoutVelocidad);
    layoutDerecho->addWidget(grupoVelocidad);

    // ===== BOTONES DE ACCION =====
    btnDisparar = new QPushButton("DISPARAR");
    btnDisparar->setMinimumHeight(60);
    btnDisparar->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "   border: 3px solid #45a049;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #CCCCCC;"
        "   color: #666666;"
        "   border: 3px solid #AAAAAA;"
        "}"
        );
    connect(btnDisparar, &QPushButton::clicked, this, &MainWindow::disparar);

    btnReset = new QPushButton("REINICIAR JUEGO");
    btnReset->setMinimumHeight(60);
    btnReset->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   border-radius: 10px;"
        "   padding: 15px;"
        "   border: 3px solid #0b7dda;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0a6bc4;"
        "}"
        );
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::resetearJuego);

    layoutDerecho->addWidget(btnDisparar);
    layoutDerecho->addWidget(btnReset);

    // ===== INSTRUCCIONES =====
    QGroupBox *grupoInstrucciones = new QGroupBox("INSTRUCCIONES");
    grupoInstrucciones->setStyleSheet("QGroupBox { font-weight: bold; font-size: 12px; }");
    QVBoxLayout *layoutInstruc = new QVBoxLayout();

    QLabel *textoInstrucciones = new QLabel(
        ". Ajusta el angulo (0-90°)\n"
        ". Ajusta la velocidad (200-800 px/s)\n"
        ". Presiona DISPARAR\n"
        ". Destruye muros enemigos\n"
        ". Impacta al rival para GANAR\n\n"
        " # Paredes: Rebote ELASTICO\n"
        " * Muros: Rebote INELASTICO\n"
        " $ Rival: ¡VICTORIA!"
        );
    textoInstrucciones->setWordWrap(true);
    textoInstrucciones->setStyleSheet(
        "padding: 12px; "
        "background-color: #FFF9C4; "
        "border-radius: 8px; "
        "font-size: 11px; "
        "line-height: 1.4;"
        );

    layoutInstruc->addWidget(textoInstrucciones);
    grupoInstrucciones->setLayout(layoutInstruc);
    layoutDerecho->addWidget(grupoInstrucciones);

    layoutDerecho->addStretch();

    // Agregar panel derecho al layout principal
    QWidget *panelDerecho = new QWidget();
    panelDerecho->setLayout(layoutDerecho);
    panelDerecho->setMaximumWidth(380);
    panelDerecho->setStyleSheet("background-color: #FAFAFA; padding: 10px; border-radius: 10px;");

    QHBoxLayout *layoutPrincipal = qobject_cast<QHBoxLayout*>(centralWidget()->layout());
    if (layoutPrincipal) {
        layoutPrincipal->addWidget(panelDerecho);
    }
}

// === CONFIGURACION DE LA ESCENA ===

void MainWindow::configurarEscena()
{
    // Crear motor de colisiones
    motorColisiones = new MotorColisiones(ANCHO_ESCENARIO, ALTO_ESCENARIO, 0.7);

    // Crear elementos estaticos visuales
    crearElementosEstaticos();

    // Crear muros
    crearMuros();

    // Textos informativos en la escena
    textoTurno = scene->addText("");
    QFont fuenteInfo("Arial", 14, QFont::Bold);
    textoTurno->setFont(fuenteInfo);
    textoTurno->setDefaultTextColor(Qt::white);
    textoTurno->setPos(10, 10);
    textoTurno->setZValue(100);

    textoEstado = scene->addText("");
    textoEstado->setFont(fuenteInfo);
    textoEstado->setDefaultTextColor(QColor(255, 255, 0));
    textoEstado->setPos(ANCHO_ESCENARIO / 2 - 100, 10);
    textoEstado->setZValue(100);

    actualizarVisualizacion();
}

void MainWindow::crearElementosEstaticos()
{
    // Borde del escenario
    QPen penEscenario(Qt::black, 5);
    itemEscenario = scene->addRect(0, 0, ANCHO_ESCENARIO, ALTO_ESCENARIO, penEscenario, Qt::NoBrush);
    itemEscenario->setZValue(1);

    // Suelo
    itemSuelo = scene->addRect(
        0, ALTO_ESCENARIO - 30, ANCHO_ESCENARIO, 30,
        QPen(Qt::darkGreen, 2), QBrush(QColor(139, 69, 19))
        );
    itemSuelo->setZValue(2);

    // Decoracion: pasto sobre el suelo
    QGraphicsRectItem *pasto = scene->addRect(
        0, ALTO_ESCENARIO - 35, ANCHO_ESCENARIO, 5,
        Qt::NoPen, QBrush(QColor(34, 139, 34))
        );
    pasto->setZValue(3);

    // Dibujar cañones
    dibujarCanon(itemCanonJ1, tuboCanonJ1, posCanonJ1, 1);
    dibujarCanon(itemCanonJ2, tuboCanonJ2, posCanonJ2, 2);

    // Dibujar rivales
    dibujarRival(itemRivalJ1, posRivalJ1, 1);
    dibujarRival(itemRivalJ2, posRivalJ2, 2);
}

void MainWindow::dibujarCanon(QGraphicsRectItem* &baseCanon, QGraphicsLineItem* &tubo, const QPointF &pos, int jugador)
{
    QColor color = obtenerColorJugador(jugador);

    // Base del cañon (rectangulo)
    baseCanon = scene->addRect(
        pos.x() - 20, pos.y() - 12, 40, 24,
        QPen(Qt::black, 3), QBrush(color)
        );
    baseCanon->setZValue(10);

    // Tubo del cañon (linea)
    double anguloTubo = (jugador == 1) ? 45 : 135;
    double largo = 30;
    double rad = anguloTubo * M_PI / 180.0;

    tubo = scene->addLine(
        pos.x(), pos.y(),
        pos.x() + largo * cos(rad), pos.y() - largo * sin(rad),
        QPen(color.darker(120), 5)
        );
    tubo->setZValue(11);

    // Ruedas del cañon (decoracion)
    scene->addEllipse(pos.x() - 25, pos.y() + 5, 12, 12, QPen(Qt::black, 2), QBrush(Qt::darkGray))->setZValue(9);
    scene->addEllipse(pos.x() + 13, pos.y() + 5, 12, 12, QPen(Qt::black, 2), QBrush(Qt::darkGray))->setZValue(9);
}

void MainWindow::dibujarRival(QGraphicsEllipseItem* &rival, const QPointF &pos, int jugador)
{
    QColor color = obtenerColorJugador(jugador);

    // Cuerpo (circulo principal)
    rival = scene->addEllipse(
        pos.x() - 15, pos.y() - 15, 30, 30,
        QPen(Qt::black, 3), QBrush(color)
        );
    rival->setZValue(10);

    // Cabeza
    QGraphicsEllipseItem *cabeza = scene->addEllipse(
        pos.x() - 12, pos.y() - 38, 24, 24,
        QPen(Qt::black, 3), QBrush(color.lighter(120))
        );
    cabeza->setZValue(10);

    // Ojos
    scene->addEllipse(pos.x() - 8, pos.y() - 32, 5, 5, Qt::NoPen, QBrush(Qt::black))->setZValue(11);
    scene->addEllipse(pos.x() + 3, pos.y() - 32, 5, 5, Qt::NoPen, QBrush(Qt::black))->setZValue(11);

    // Boca (sonrisa)
    QPainterPath boca;
    boca.arcMoveTo(pos.x() - 8, pos.y() - 28, 16, 10, 180);
    boca.arcTo(pos.x() - 8, pos.y() - 28, 16, 10, 180, 180);
    scene->addPath(boca, QPen(Qt::black, 2))->setZValue(11);

    // Brazos
    QPen penExtremidades(color.darker(130), 4);
    scene->addLine(pos.x() - 15, pos.y() - 5, pos.x() - 28, pos.y() + 5, penExtremidades)->setZValue(9);
    scene->addLine(pos.x() + 15, pos.y() - 5, pos.x() + 28, pos.y() + 5, penExtremidades)->setZValue(9);

    // Piernas
    scene->addLine(pos.x() - 8, pos.y() + 15, pos.x() - 12, pos.y() + 35, penExtremidades)->setZValue(9);
    scene->addLine(pos.x() + 8, pos.y() + 15, pos.x() + 12, pos.y() + 35, penExtremidades)->setZValue(9);
}

void MainWindow::crearMuros()
{
    // Configuracion de muros para Jugador 1 (lado izquierdo)
    double xBase1 = 100;
    double yBase1 = ALTO_ESCENARIO - 150;

    muros.push_back(new Muro(xBase1 - 50, yBase1 - 100, 40, 100, 100, 1, 0.7));    // Superior
    muros.push_back(new Muro(xBase1 - 50, yBase1, 100, 40, 200, 1, 0.7));          // Izquierdo
    muros.push_back(new Muro(xBase1 + 50, yBase1, 100, 40, 200, 1, 0.7));          // Derecho

    // Configuracion de muros para Jugador 2 (lado derecho)
    double xBase2 = ANCHO_ESCENARIO - 100;
    double yBase2 = ALTO_ESCENARIO - 150;

    muros.push_back(new Muro(xBase2 + 10, yBase2 - 100, 40, 100, 100, 2, 0.7));    // Superior
    muros.push_back(new Muro(xBase2 - 150, yBase2, 100, 40, 200, 2, 0.7));         // Izquierdo
    muros.push_back(new Muro(xBase2 - 50, yBase2, 100, 40, 200, 2, 0.7));          // Derecho

    // Crear items graficos para los muros
    for (size_t i = 0; i < muros.size(); i++) {
        Muro *muro = muros[i];
        QColor colorMuro = obtenerColorJugador(muro->getIdPropietario()).lighter(130);

        QGraphicsRectItem *item = scene->addRect(
            muro->getX(), muro->getY(),
            muro->getAncho(), muro->getAlto(),
            QPen(Qt::black, 3), QBrush(colorMuro)
            );
        item->setZValue(5);
        itemsMuros.push_back(item);

        // Texto de vida del muro
        QGraphicsTextItem *textoVida = scene->addText("");
        QFont fuenteVida("Arial", 11, QFont::Bold);
        textoVida->setFont(fuenteVida);
        textoVida->setDefaultTextColor(Qt::white);
        textoVida->setZValue(6);
        textosVidaMuros.push_back(textoVida);
    }

    actualizarBarrasVida();
}

void MainWindow::actualizarBarrasVida()
{
    for (size_t i = 0; i < muros.size(); i++) {
        if (i < textosVidaMuros.size() && muros[i]) {
            double porcentaje = muros[i]->getPorcentajeVida();
            int vida = static_cast<int>(muros[i]->getResistencia());

            QString texto = QString("%1").arg(vida);
            textosVidaMuros[i]->setPlainText(texto);

            // Centrar texto en el muro
            double x = muros[i]->getX() + muros[i]->getAncho() / 2 - textosVidaMuros[i]->boundingRect().width() / 2;
            double y = muros[i]->getY() + muros[i]->getAlto() / 2 - textosVidaMuros[i]->boundingRect().height() / 2;
            textosVidaMuros[i]->setPos(x, y);

            // Cambiar color del muro segun porcentaje de vida
            if (i < itemsMuros.size() && itemsMuros[i]) {
                QColor colorBase = obtenerColorJugador(muros[i]->getIdPropietario());

                if (porcentaje > 0.7) {
                    itemsMuros[i]->setBrush(QBrush(colorBase.lighter(130)));
                } else if (porcentaje > 0.3) {
                    itemsMuros[i]->setBrush(QBrush(QColor(255, 165, 0))); // Naranja
                } else {
                    itemsMuros[i]->setBrush(QBrush(QColor(220, 20, 60))); // Rojo
                }

                // Ocultar muros destruidos
                if (muros[i]->estaDestruido()) {
                    itemsMuros[i]->setVisible(false);
                    textosVidaMuros[i]->setVisible(false);
                }
            }
        }
    }
}

// ==== ACTUALIZACION DE INFORMACION ====

void MainWindow::actualizarInfoAngulo(int valor)
{
    labelAngulo->setText(QString("Ángulo: %1°").arg(valor));
}

void MainWindow::actualizarInfoVelocidad(int valor)
{
    labelVelocidad->setText(QString("Velocidad: %1 px/s").arg(valor));
}

void MainWindow::actualizarVisualizacion()
{
    // Actualizar posicion del proyectil
    if (proyectilActivo && itemProyectil && proyectilActivo->estaActiva()) {
        double x = proyectilActivo->getX();
        double y = proyectilActivo->getY();
        itemProyectil->setPos(x - 6, y - 6);
    }

    // Actualizar texto de informacion
    QString nombreJugador = (turnoActual == 1) ? "Jugador 1" : "Jugador 2";
    textoTurno->setPlainText(QString("Turno: %1").arg(nombreJugador));
}

void MainWindow::limpiarProyectil()
{
    if (itemProyectil) {
        scene->removeItem(itemProyectil);
        delete itemProyectil;
        itemProyectil = nullptr;
    }

    if (proyectilActivo) {
        delete proyectilActivo;
        proyectilActivo = nullptr;
    }
}

QColor MainWindow::obtenerColorJugador(int jugador)
{
    return (jugador == 1) ? QColor(255, 68, 68) : QColor(68, 138, 255);
}

// ==== SLOTS DE INTERACCION ====

void MainWindow::disparar()
{
    if (!juegoActivo || proyectilEnMovimiento) {
        return;
    }

    // Obtener parametros de disparo
    double angulo = sliderAngulo->value();
    double velocidad = sliderVelocidad->value();

    // Determinar posicion inicial segun turno
    QPointF posInicial;
    if (turnoActual == 1) {
        posInicial = posCanonJ1;
    } else {
        posInicial = posCanonJ2;
        angulo = 180 - angulo; // Invertir angulo para jugador 2
    }

    // Crear proyectil
    proyectilActivo = new Proyectil(
        posInicial.x(), posInicial.y(),
        angulo, velocidad,
        MASA_PROYECTIL, turnoActual, GRAVEDAD
        );

    // Crear item grafico del proyectil
    QColor colorProyectil = obtenerColorJugador(turnoActual).darker(110);
    itemProyectil = scene->addEllipse(
        -6, -6, 12, 12,
        QPen(Qt::black, 2), QBrush(colorProyectil)
        );
    itemProyectil->setZValue(20);

    // Iniciar animacion
    proyectilEnMovimiento = true;
    btnDisparar->setEnabled(false);
    timer->start(16); // ~60 FPS

    textoEstado->setPlainText("💥 Proyectil en vuelo...");
}

void MainWindow::actualizarSimulacion()
{
    if (!proyectilActivo || !proyectilActivo->estaActiva()) {
        return;
    }

    // Actualizar fisica del proyectil
    proyectilActivo->actualizarMovimiento(DT);

    // Verificar colisiones
    verificarColisiones();

    // Actualizar visualizacion
    actualizarVisualizacion();

    // Verificar si el proyectil salio del escenario
    double x = proyectilActivo->getX();
    double y = proyectilActivo->getY();

    if (x < -100 || x > ANCHO_ESCENARIO + 100 || y > ALTO_ESCENARIO + 100) {
        limpiarProyectil();
        cambiarTurno();
    }
}

void MainWindow::resetearJuego()
{
    // Detener timer
    timer->stop();

    // Limpiar proyectil
    limpiarProyectil();

    // Restaurar muros
    for (auto muro : muros) {
        muro->restaurar();
    }

    // Restaurar visibilidad de muros
    for (auto item : itemsMuros) {
        item->setVisible(true);
    }
    for (auto texto : textosVidaMuros) {
        texto->setVisible(true);
    }

    actualizarBarrasVida();

    // Resetear estado del juego
    turnoActual = 1;
    juegoActivo = true;
    proyectilEnMovimiento = false;

    // Resetear controles
    sliderAngulo->setValue(45);
    sliderVelocidad->setValue(500);
    btnDisparar->setEnabled(true);

    labelTurno->setText("JUGADOR 1");
    labelTurno->setStyleSheet(
        "color: #FF4444; "
        "background-color: #FFE6E6; "
        "padding: 20px; "
        "border-radius: 10px; "
        "border: 3px solid #FF4444;"
        );

    textoEstado->setPlainText(" Nuevo juego - ¡Buena suerte!");

    actualizarVisualizacion();
}

void MainWindow::verificarColisiones()
{

}

void MainWindow::cambiarTurno()
{
    timer->stop();
    proyectilEnMovimiento = false;

    turnoActual = (turnoActual == 1) ? 2 : 1;

    // Actualizar UI
    QColor colorTurno = obtenerColorJugador(turnoActual);
    QString bgColor = (turnoActual == 1) ? "#FFE6E6" : "#E6F3FF";
    QString borderColor = (turnoActual == 1) ? "#FF4444" : "#4488FF";

    labelTurno->setText(QString("JUGADOR #1").arg(turnoActual));
    labelTurno->setStyleSheet(QString(
                                  "color: %1; "
                                  "background-color: %2; "
                                  "padding: 20px; "
                                  "border-radius: 10px; "
                                  "border: 3px solid %3;"
                                  ).arg(colorTurno.name()).arg(bgColor).arg(borderColor));

    btnDisparar->setEnabled(true);
    textoEstado->setPlainText("Tu turno - ¡Ajusta y dispara!");
}

void MainWindow::mostrarVictoria(int jugadorGanador)
{
    timer->stop();
    juegoActivo = false;
    btnDisparar->setEnabled(false);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("¡VICTORIA!");
    msgBox.setText(QString("¡JUGADOR #1 HA GANADO!").arg(jugadorGanador));
    msgBox.setInformativeText("Ha impactado al rival enemigo.\n\n¿Deseas jugar otra vez?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FFF8E1; }"
        "QPushButton { min-width: 80px; min-height: 30px; font-size: 14px; }"
        );

    int respuesta = msgBox.exec();

    if (respuesta == QMessageBox::Yes) {
        resetearJuego();
    }
}
