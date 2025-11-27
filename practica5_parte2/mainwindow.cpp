#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(nullptr)
    , view(nullptr)
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
    , textoInstrucciones(nullptr)
    , sliderAngulo(nullptr)
    , sliderVelocidad(nullptr)
    , spinAngulo(nullptr)
    , spinVelocidad(nullptr)
    , btnDisparar(nullptr)
    , btnReset(nullptr)
    , labelTurno(nullptr)
    , labelAngulo(nullptr)
    , labelVelocidad(nullptr)
    , timer(nullptr)
    , proyectilActivo(nullptr)
    , motorColisiones(nullptr)
    , turnoActual(1)
    , juegoActivo(true)
    , proyectilEnMovimiento(false)
{
    ui->setupUi(this);

    configurarInterfaz();
    configurarEscena();
    crearElementosEstaticos();
    crearMuros();

    // Timer para animacion
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarSimulacion);

    // Motor de colisiones
    motorColisiones = new MotorColisiones(ANCHO_ESCENARIO, ALTO_ESCENARIO, 0.7);

    actualizarVisualizacion();
}

MainWindow::~MainWindow()
{
    if (timer) {
        timer->stop();
        delete timer;
    }

    if (proyectilActivo) {
        delete proyectilActivo;
    }

    for (auto muro : muros) {
        delete muro;
    }

    if (motorColisiones) {
        delete motorColisiones;
    }

    delete ui;
}

void MainWindow::configurarInterfaz()
{
    // Widget central con layout horizontal
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(10);

    // Vista grafica
    view = new QGraphicsView(centralWidget);
    view->setFixedSize(1050, 650);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(view);

    // Panel de controles
    QWidget *panelControles = new QWidget(centralWidget);
    panelControles->setFixedWidth(230);
    QVBoxLayout *layoutControles = new QVBoxLayout(panelControles);

    layoutControles->addStretch();

    // Label de turno
    labelTurno = new QLabel("Turno: Jugador 1", panelControles);
    labelTurno->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3;");
    labelTurno->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelTurno);

    layoutControles->addSpacing(30);

    // Control de angulo
    labelAngulo = new QLabel("Ángulo: 45°", panelControles);
    labelAngulo->setStyleSheet("font-size: 14px;");
    labelAngulo->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelAngulo);

    sliderAngulo = new QSlider(Qt::Horizontal, panelControles);
    sliderAngulo->setRange(0, 90);
    sliderAngulo->setValue(45);
    layoutControles->addWidget(sliderAngulo);

    spinAngulo = new QSpinBox(panelControles);
    spinAngulo->setRange(0, 90);
    spinAngulo->setValue(45);
    spinAngulo->setSuffix("°");
    layoutControles->addWidget(spinAngulo);

    layoutControles->addSpacing(20);

    // Control de velocidad
    labelVelocidad = new QLabel("Velocidad: 400", panelControles);
    labelVelocidad->setStyleSheet("font-size: 14px;");
    labelVelocidad->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelVelocidad);

    sliderVelocidad = new QSlider(Qt::Horizontal, panelControles);
    sliderVelocidad->setRange(100, 800);
    sliderVelocidad->setValue(400);
    layoutControles->addWidget(sliderVelocidad);

    spinVelocidad = new QSpinBox(panelControles);
    spinVelocidad->setRange(100, 800);
    spinVelocidad->setValue(400);
    layoutControles->addWidget(spinVelocidad);

    layoutControles->addSpacing(30);

    // Botones
    btnDisparar = new QPushButton("Disparar (ESPACIO)", panelControles);
    btnDisparar->setStyleSheet("font-size: 14px; padding: 12px; background-color: #4CAF50; "
                               "color: white; border-radius: 5px; font-weight: bold;");
    layoutControles->addWidget(btnDisparar);

    btnReset = new QPushButton("Reiniciar (R)", panelControles);
    btnReset->setStyleSheet("font-size: 14px; padding: 12px; background-color: #FF5722; "
                            "color: white; border-radius: 5px; font-weight: bold;");
    layoutControles->addWidget(btnReset);

    layoutControles->addStretch();

    mainLayout->addWidget(panelControles);
    setCentralWidget(centralWidget);

    // Conectar señales
    connect(sliderAngulo, &QSlider::valueChanged, this, &MainWindow::actualizarInfoAngulo);
    connect(spinAngulo, QOverload<int>::of(&QSpinBox::valueChanged), sliderAngulo, &QSlider::setValue);
    connect(sliderVelocidad, &QSlider::valueChanged, this, &MainWindow::actualizarInfoVelocidad);
    connect(spinVelocidad, QOverload<int>::of(&QSpinBox::valueChanged), sliderVelocidad, &QSlider::setValue);
    connect(btnDisparar, &QPushButton::clicked, this, &MainWindow::disparar);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::resetearJuego);
}

void MainWindow::configurarEscena()
{
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, ANCHO_ESCENARIO, ALTO_ESCENARIO);
    scene->setBackgroundBrush(QBrush(QColor(135, 206, 235)));

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::crearElementosEstaticos()
{
    // Suelo
    itemSuelo = scene->addRect(0, ALTO_ESCENARIO - 50, ANCHO_ESCENARIO, 50,
                               QPen(Qt::NoPen),
                               QBrush(QColor(139, 69, 19)));

    // Posiciones
    posCanonJ1 = QPointF(80, ALTO_ESCENARIO - 80);
    posCanonJ2 = QPointF(ANCHO_ESCENARIO - 80, ALTO_ESCENARIO - 80);
    posRivalJ1 = QPointF(80, ALTO_ESCENARIO - 200);
    posRivalJ2 = QPointF(ANCHO_ESCENARIO - 80, ALTO_ESCENARIO - 200);

    // Dibujar cañones
    dibujarCanon(itemCanonJ1, tuboCanonJ1, posCanonJ1, 1);
    dibujarCanon(itemCanonJ2, tuboCanonJ2, posCanonJ2, 2);

    // Dibujar rivales
    dibujarRival(itemRivalJ1, posRivalJ1, 1);
    dibujarRival(itemRivalJ2, posRivalJ2, 2);

    // Textos
    textoTurno = scene->addText("Turno: Jugador 1");
    textoTurno->setDefaultTextColor(Qt::white);
    textoTurno->setFont(QFont("Arial", 16, QFont::Bold));
    textoTurno->setPos(10, 10);

    textoEstado = scene->addText("Listo para disparar");
    textoEstado->setDefaultTextColor(Qt::yellow);
    textoEstado->setFont(QFont("Arial", 12));
    textoEstado->setPos(10, 45);

    textoInstrucciones = scene->addText("↑↓: Ángulo | ESPACIO: Disparar | R: Reiniciar");
    textoInstrucciones->setDefaultTextColor(Qt::white);
    textoInstrucciones->setFont(QFont("Arial", 11));
    textoInstrucciones->setPos(10, ALTO_ESCENARIO - 35);
}

void MainWindow::crearMuros()
{
    // Configuracion de muros
    struct ConfigMuro {
        double x, y, ancho, alto, resistencia;
        int propietario;
    };

    std::vector<ConfigMuro> configMuros = {
        // Muros Jugador 1 (izquierda)
        {30, ALTO_ESCENARIO - 280, 40, 150, 200.0, 1},   // Izquierdo
        {150, ALTO_ESCENARIO - 280, 40, 150, 200.0, 1},  // Derecho
        {50, ALTO_ESCENARIO - 320, 120, 40, 100.0, 1},   // Superior

        // Muros Jugador 2 (derecha)
        {ANCHO_ESCENARIO - 190, ALTO_ESCENARIO - 280, 40, 150, 200.0, 2}, // Izquierdo
        {ANCHO_ESCENARIO - 70, ALTO_ESCENARIO - 280, 40, 150, 200.0, 2},  // Derecho
        {ANCHO_ESCENARIO - 170, ALTO_ESCENARIO - 320, 120, 40, 100.0, 2}  // Superior
    };

    for (const auto& config : configMuros) {
        Muro* muro = new Muro(config.x, config.y, config.ancho, config.alto,
                              config.resistencia, config.propietario, 0.7);
        muros.push_back(muro);

        QColor colorMuro = (config.propietario == 1) ?
                               QColor(100, 149, 237) : QColor(220, 20, 60);

        QGraphicsRectItem* itemMuro = scene->addRect(config.x, config.y,
                                                     config.ancho, config.alto,
                                                     QPen(Qt::black, 2),
                                                     QBrush(colorMuro));
        itemsMuros.push_back(itemMuro);

        QGraphicsTextItem* textoVida = scene->addText(QString::number((int)config.resistencia));
        textoVida->setDefaultTextColor(Qt::white);
        textoVida->setFont(QFont("Arial", 11, QFont::Bold));

        double textX = config.x + config.ancho/2 - 15;
        double textY = config.y + config.alto/2 - 12;
        textoVida->setPos(textX, textY);
        textosVidaMuros.push_back(textoVida);
    }
}

void MainWindow::dibujarCanon(QGraphicsRectItem* &baseCanon, QGraphicsLineItem* &tubo,
                              const QPointF &pos, int jugador)
{
    QColor color = obtenerColorJugador(jugador);

    // Base
    baseCanon = scene->addRect(pos.x() - 15, pos.y() - 10, 30, 20,
                               QPen(Qt::black, 2),
                               QBrush(color));

    // Tubo
    double longitud = 45;
    double angulo = (jugador == 1) ? 45 : 135;
    double anguloRad = angulo * M_PI / 180.0;
    double xFin = pos.x() + longitud * cos(anguloRad);
    double yFin = pos.y() - longitud * sin(anguloRad);

    tubo = scene->addLine(pos.x(), pos.y(), xFin, yFin,
                          QPen(color, 6, Qt::SolidLine, Qt::RoundCap));
}

void MainWindow::dibujarRival(QGraphicsEllipseItem* &rival, const QPointF &pos, int jugador)
{
    QColor color = obtenerColorJugador(jugador);

    // Cuerpo (circulo)
    rival = scene->addEllipse(pos.x() - 20, pos.y() - 40, 40, 40,
                              QPen(Qt::black, 2),
                              QBrush(color));

    // Cabeza
    scene->addEllipse(pos.x() - 10, pos.y() - 60, 20, 20,
                      QPen(Qt::black, 2),
                      QBrush(color.lighter(120)));

    // Brazos
    scene->addLine(pos.x() - 20, pos.y() - 25, pos.x() - 30, pos.y() - 15,
                   QPen(Qt::black, 2));
    scene->addLine(pos.x() + 20, pos.y() - 25, pos.x() + 30, pos.y() - 15,
                   QPen(Qt::black, 2));

    // Piernas
    scene->addLine(pos.x(), pos.y(), pos.x() - 15, pos.y() + 25,
                   QPen(Qt::black, 2));
    scene->addLine(pos.x(), pos.y(), pos.x() + 15, pos.y() + 25,
                   QPen(Qt::black, 2));
}

QColor MainWindow::obtenerColorJugador(int jugador)
{
    return (jugador == 1) ? QColor(65, 105, 225) : QColor(220, 20, 60);
}

void MainWindow::disparar()
{
    if (!juegoActivo || proyectilEnMovimiento) {
        return;
    }

    double angulo = sliderAngulo->value();
    double velocidad = sliderVelocidad->value();

    // Ajustar angulo segun jugador
    if (turnoActual == 2) {
        angulo = 180 - angulo;
    }

    QPointF posInicial = (turnoActual == 1) ? posCanonJ1 : posCanonJ2;

    proyectilActivo = new Proyectil(posInicial.x(), posInicial.y(),
                                    angulo, velocidad,
                                    MASA_PROYECTIL, turnoActual, GRAVEDAD);

    itemProyectil = scene->addEllipse(0, 0, 12, 12,
                                      QPen(Qt::black, 2),
                                      QBrush(Qt::red));
    itemProyectil->setPos(posInicial);

    proyectilEnMovimiento = true;
    timer->start(16);

    textoEstado->setPlainText("Proyectil en vuelo...");
    textoEstado->setDefaultTextColor(Qt::yellow);
    btnDisparar->setEnabled(false);
}

void MainWindow::actualizarSimulacion()
{
    if (!proyectilActivo || !proyectilActivo->estaActiva()) {
        timer->stop();
        limpiarProyectil();
        cambiarTurno();
        return;
    }

    proyectilActivo->actualizarMovimiento(DT);
    verificarColisiones();

    if (proyectilActivo && proyectilActivo->estaActiva() && itemProyectil) {
        itemProyectil->setPos(proyectilActivo->getX(), proyectilActivo->getY());
    }
}

void MainWindow::verificarColisiones()
{
    if (!proyectilActivo || !motorColisiones) return;

    InfoColision info = motorColisiones->verificarColisiones(
        *proyectilActivo, muros, posRivalJ1, posRivalJ2
        );

    if (info.ocurrioColision) {
        if (info.tipoColision == "rival") {
            mostrarVictoria(turnoActual);
        }
        else if (info.tipoColision == "muro") {
            int indiceMuro = info.indiceObjetoImpactado;
            if (indiceMuro >= 0 && indiceMuro < (int)muros.size()) {
                double danio = FACTOR_DANIO * info.momentoImpacto;
                muros[indiceMuro]->recibirDanio(danio);
                actualizarBarrasVida();
            }
        }
    }
}

void MainWindow::actualizarBarrasVida()
{
    for (size_t i = 0; i < muros.size(); i++) {
        if (i < textosVidaMuros.size()) {
            int vida = (int)muros[i]->getResistencia();

            if (muros[i]->estaDestruido()) {
                itemsMuros[i]->setBrush(QBrush(QColor(50, 50, 50)));
                textosVidaMuros[i]->setPlainText("X");
                textosVidaMuros[i]->setDefaultTextColor(Qt::red);
            } else {
                textosVidaMuros[i]->setPlainText(QString::number(vida));

                double porcentaje = muros[i]->getPorcentajeVida();
                if (porcentaje < 0.3) {
                    QColor colorDanio = (muros[i]->getIdPropietario() == 1) ?
                                            QColor(150, 100, 100) : QColor(255, 100, 100);
                    itemsMuros[i]->setBrush(QBrush(colorDanio));
                }
            }
        }
    }
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

    proyectilEnMovimiento = false;
    btnDisparar->setEnabled(true);
}

void MainWindow::cambiarTurno()
{
    turnoActual = (turnoActual == 1) ? 2 : 1;
    actualizarVisualizacion();
    textoEstado->setPlainText("Listo para disparar");
    textoEstado->setDefaultTextColor(Qt::yellow);
}

void MainWindow::actualizarVisualizacion()
{
    QString turnoTexto = QString("Turno: Jugador %1").arg(turnoActual);
    textoTurno->setPlainText(turnoTexto);
    labelTurno->setText(turnoTexto);

    QColor color = obtenerColorJugador(turnoActual);
    labelTurno->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;")
                                  .arg(color.name()));

    actualizarAnguloCanon();
}

void MainWindow::actualizarAnguloCanon()
{
    double angulo = sliderAngulo->value();
    double longitud = 45;

    QPointF pos;
    QGraphicsLineItem* tubo;

    if (turnoActual == 1) {
        pos = posCanonJ1;
        tubo = tuboCanonJ1;
    } else {
        pos = posCanonJ2;
        tubo = tuboCanonJ2;
        angulo = 180 - angulo;
    }

    double anguloRad = angulo * M_PI / 180.0;
    double xFin = pos.x() + longitud * cos(anguloRad);
    double yFin = pos.y() - longitud * sin(anguloRad);

    tubo->setLine(pos.x(), pos.y(), xFin, yFin);
}

void MainWindow::actualizarInfoAngulo(int valor)
{
    labelAngulo->setText(QString("Ángulo: %1°").arg(valor));
    spinAngulo->setValue(valor);
    actualizarAnguloCanon();
}

void MainWindow::actualizarInfoVelocidad(int valor)
{
    labelVelocidad->setText(QString("Velocidad: %1").arg(valor));
    spinVelocidad->setValue(valor);
}

void MainWindow::resetearJuego()
{
    if (timer) {
        timer->stop();
    }

    limpiarProyectil();

    // Restaurar muros
    for (size_t i = 0; i < muros.size(); i++) {
        muros[i]->restaurar();
        if (i < textosVidaMuros.size()) {
            int vida = (int)muros[i]->getResistenciaMaxima();
            textosVidaMuros[i]->setPlainText(QString::number(vida));
            textosVidaMuros[i]->setDefaultTextColor(Qt::white);

            QColor colorMuro = (muros[i]->getIdPropietario() == 1) ?
                                   QColor(100, 149, 237) : QColor(220, 20, 60);
            itemsMuros[i]->setBrush(QBrush(colorMuro));
        }
    }

    turnoActual = 1;
    juegoActivo = true;
    proyectilEnMovimiento = false;

    sliderAngulo->setValue(45);
    sliderVelocidad->setValue(400);
    btnDisparar->setEnabled(true);

    actualizarVisualizacion();
    textoEstado->setPlainText("Juego reiniciado");
    textoEstado->setDefaultTextColor(Qt::green);
}

void MainWindow::mostrarVictoria(int jugadorGanador)
{
    timer->stop();
    juegoActivo = false;

    QString mensaje = QString("¡Jugador %1 ha ganado!").arg(jugadorGanador);
    textoEstado->setPlainText(mensaje);
    textoEstado->setDefaultTextColor(Qt::green);
    textoEstado->setFont(QFont("Arial", 14, QFont::Bold));

    QMessageBox::information(this, "¡Victoria!",
                             mensaje + "\n\nPresiona R para reiniciar.");

    btnDisparar->setEnabled(false);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!juegoActivo && event->key() != Qt::Key_R) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up:
        if (sliderAngulo->value() < 90) {
            sliderAngulo->setValue(sliderAngulo->value() + 1);
        }
        break;

    case Qt::Key_Down:
        if (sliderAngulo->value() > 0) {
            sliderAngulo->setValue(sliderAngulo->value() - 1);
        }
        break;

    case Qt::Key_Space:
        if (!proyectilEnMovimiento) {
            disparar();
        }
        break;

    case Qt::Key_R:
        resetearJuego();
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}
