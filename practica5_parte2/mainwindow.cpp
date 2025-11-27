
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
    , labelTurno(nullptr)
    , labelAngulo(nullptr)
    , labelVelocidad(nullptr)
    , labelEstadoColision(nullptr)
    , timer(nullptr)
    , timerTimeout(nullptr)
    , proyectilActivo(nullptr)
    , motorColisiones(nullptr)
    , turnoActual(1)
    , juegoActivo(true)
    , proyectilEnMovimiento(false)
    , ultimoTipoColision("")
    , contadorColisionesElasticas(0)
    , tiempoVueloProyectil(0.0)
{
    ui->setupUi(this);

    configurarInterfaz();
    configurarEscena();
    crearElementosEstaticos();
    crearMuros();

    // Timer para animacion
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarSimulacion);

    // Timer para timeout de 15 segundos
    timerTimeout = new QTimer(this);
    timerTimeout->setSingleShot(true);
    connect(timerTimeout, &QTimer::timeout, this, &MainWindow::verificarTimeout);

    // Motor de colisiones
    motorColisiones = new MotorColisiones(ANCHO_ESCENARIO, ALTO_ESCENARIO, 0.7);

    actualizarVisualizacion();

    // Configurar foco para teclado
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

MainWindow::~MainWindow()
{
    if (timer) {
        timer->stop();
        delete timer;
    }

    if (timerTimeout) {
        timerTimeout->stop();
        delete timerTimeout;
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
    view->setFocusPolicy(Qt::NoFocus);
    mainLayout->addWidget(view);

    // Panel de controles
    QWidget *panelControles = new QWidget(centralWidget);
    panelControles->setFixedWidth(250);
    panelControles->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout *layoutControles = new QVBoxLayout(panelControles);

    layoutControles->addStretch();

    // Label de turno
    labelTurno = new QLabel("Turno: Jugador 1", panelControles);
    labelTurno->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3;");
    labelTurno->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelTurno);

    layoutControles->addSpacing(20);

    // Estado de colision
    QLabel *tituloEstado = new QLabel("ESTADO:", panelControles);
    tituloEstado->setStyleSheet("font-size: 12px; font-weight: bold;");
    tituloEstado->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(tituloEstado);

    labelEstadoColision = new QLabel("Esperando disparo...", panelControles);
    labelEstadoColision->setStyleSheet(
        "background-color: #f0f0f0; "
        "padding: 10px; "
        "border-radius: 5px; "
        "font-size: 11px; "
        "font-weight: bold; "
        "color: #666;"
    );
    labelEstadoColision->setAlignment(Qt::AlignCenter);
    labelEstadoColision->setWordWrap(true);
    labelEstadoColision->setMinimumHeight(50);
    layoutControles->addWidget(labelEstadoColision);

    layoutControles->addSpacing(30);

    // Control de angulo
    labelAngulo = new QLabel("Angulo: 45°", panelControles);
    labelAngulo->setStyleSheet("font-size: 14px; font-weight: bold;");
    labelAngulo->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelAngulo);

    sliderAngulo = new QSlider(Qt::Horizontal, panelControles);
    sliderAngulo->setRange(0, 90);
    sliderAngulo->setValue(45);
    sliderAngulo->setFocusPolicy(Qt::NoFocus);
    layoutControles->addWidget(sliderAngulo);

    spinAngulo = new QSpinBox(panelControles);
    spinAngulo->setRange(0, 90);
    spinAngulo->setValue(45);
    spinAngulo->setSuffix("°");
    spinAngulo->setAlignment(Qt::AlignCenter);
    spinAngulo->setStyleSheet("font-size: 12px; padding: 3px;");
    spinAngulo->setFocusPolicy(Qt::NoFocus);
    layoutControles->addWidget(spinAngulo);

    layoutControles->addSpacing(20);

    // Control de velocidad
    labelVelocidad = new QLabel("Velocidad: 400 m/s", panelControles);
    labelVelocidad->setStyleSheet("font-size: 14px; font-weight: bold;");
    labelVelocidad->setAlignment(Qt::AlignCenter);
    layoutControles->addWidget(labelVelocidad);

    sliderVelocidad = new QSlider(Qt::Horizontal, panelControles);
    sliderVelocidad->setRange(100, 800);
    sliderVelocidad->setValue(400);
    sliderVelocidad->setFocusPolicy(Qt::NoFocus);
    layoutControles->addWidget(sliderVelocidad);

    spinVelocidad = new QSpinBox(panelControles);
    spinVelocidad->setRange(100, 800);
    spinVelocidad->setValue(400);
    spinVelocidad->setSuffix(" m/s");
    spinVelocidad->setAlignment(Qt::AlignCenter);
    spinVelocidad->setStyleSheet("font-size: 12px; padding: 3px;");
    spinVelocidad->setFocusPolicy(Qt::NoFocus);
    layoutControles->addWidget(spinVelocidad);

    layoutControles->addSpacing(30);

    // Instrucciones
    QLabel *labelInstrucciones = new QLabel(
        "<b>CONTROLES:</b><br>"
        "↑↓: Ajustar angulo<br>"
        "←→: Ajustar velocidad<br>"
        "ESPACIO: Disparar<br>"
        "R: Reiniciar",
        panelControles
        );
    labelInstrucciones->setStyleSheet(
        "background-color: #e3f2fd; "
        "padding: 10px; "
        "border-radius: 5px; "
        "font-size: 11px;"
        );
    labelInstrucciones->setWordWrap(true);
    layoutControles->addWidget(labelInstrucciones);

    layoutControles->addStretch();

    mainLayout->addWidget(panelControles);
    setCentralWidget(centralWidget);

    // Conectar señales
    connect(sliderAngulo, &QSlider::valueChanged, spinAngulo, &QSpinBox::setValue);
    connect(spinAngulo, QOverload<int>::of(&QSpinBox::valueChanged), sliderAngulo, &QSlider::setValue);
    connect(sliderAngulo, &QSlider::valueChanged, this, &MainWindow::actualizarInfoAngulo);

    connect(sliderVelocidad, &QSlider::valueChanged, spinVelocidad, &QSpinBox::setValue);
    connect(spinVelocidad, QOverload<int>::of(&QSpinBox::valueChanged), sliderVelocidad, &QSlider::setValue);
    connect(sliderVelocidad, &QSlider::valueChanged, this, &MainWindow::actualizarInfoVelocidad);
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
    QPen penSuelo(QColor(139, 69, 19), 3);
    scene->addLine(0, ALTO_ESCENARIO, ANCHO_ESCENARIO, ALTO_ESCENARIO, penSuelo);

    itemSuelo = scene->addRect(0, ALTO_ESCENARIO - 5, ANCHO_ESCENARIO, 5,
                               QPen(Qt::NoPen),
                               QBrush(QColor(139, 69, 19)));

    // CAÑONES ELEVADOS EN LAS PARTES SUPERIORES
    posCanonJ1 = QPointF(50, 100);   // Superior izquierda
    posCanonJ2 = QPointF(ANCHO_ESCENARIO - 50, 100); // Superior derecha

    posRivalJ1 = QPointF(110, ALTO_ESCENARIO - 60);
    posRivalJ2 = QPointF(ANCHO_ESCENARIO - 110, ALTO_ESCENARIO - 60);

    // Dibujar cañones
    dibujarCanon(itemCanonJ1, tuboCanonJ1, posCanonJ1, 1);
    dibujarCanon(itemCanonJ2, tuboCanonJ2, posCanonJ2, 2);

    // Dibujar plataformas para los cañones
    scene->addRect(posCanonJ1.x() - 25, posCanonJ1.y() + 15, 50, 10,
                   QPen(Qt::black, 2),
                   QBrush(QColor(100, 100, 100)));

    scene->addRect(posCanonJ2.x() - 25, posCanonJ2.y() + 15, 50, 10,
                   QPen(Qt::black, 2),
                   QBrush(QColor(100, 100, 100)));

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

    textoInstrucciones = scene->addText("↑↓: Ángulo | ←→: Velocidad | ESPACIO: Disparar | R: Reiniciar");
    textoInstrucciones->setDefaultTextColor(Qt::white);
    textoInstrucciones->setFont(QFont("Arial", 10));
    textoInstrucciones->setPos(10, ALTO_ESCENARIO - 25);
}

void MainWindow::crearMuros()
{

    struct ConfigMuro {
        double x, y, ancho, alto, resistencia;
        int propietario;
    };

    std::vector<ConfigMuro> configMuros = {
        // Muros Jugador 1 - Protegen rival
        {40, ALTO_ESCENARIO - 140, 40, 140, 200.0, 1},   // Izq
        {180, ALTO_ESCENARIO - 140, 40, 140, 200.0, 1},  // Der
        {60, ALTO_ESCENARIO - 170, 140, 30, 100.0, 1},   // Sup

        // Muros Jugador 2 - Protegen rival
        {ANCHO_ESCENARIO - 220, ALTO_ESCENARIO - 140, 40, 140, 200.0, 2},  // Izq
        {ANCHO_ESCENARIO - 80, ALTO_ESCENARIO - 140, 40, 140, 200.0, 2},   // Der
        {ANCHO_ESCENARIO - 200, ALTO_ESCENARIO - 170, 140, 30, 100.0, 2}   // Sup
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

    // Base del cañon mas pequeña
    baseCanon = scene->addRect(pos.x() - 12, pos.y() - 8, 24, 16,
                               QPen(Qt::black, 2),
                               QBrush(color));

    // Tubo del cañon
    double longitud = 35;
    double angulo = (jugador == 1) ? 45 : 135;
    double anguloRad = angulo * M_PI / 180.0;
    double xFin = pos.x() + longitud * cos(anguloRad);
    double yFin = pos.y() - longitud * sin(anguloRad);

    tubo = scene->addLine(pos.x(), pos.y(), xFin, yFin,
                          QPen(color, 5, Qt::SolidLine, Qt::RoundCap));
}

void MainWindow::dibujarRival(QGraphicsEllipseItem* &rival, const QPointF &pos, int jugador)
{
    QColor color = obtenerColorJugador(jugador);

    // Cuerpo
    rival = scene->addEllipse(pos.x() - 20, pos.y(), 40, 40,
                              QPen(Qt::black, 2),
                              QBrush(color));

    // Cabeza
    scene->addEllipse(pos.x() - 10, pos.y() - 20, 20, 20,
                      QPen(Qt::black, 2),
                      QBrush(color.lighter(120)));

    // Brazos
    scene->addLine(pos.x() - 20, pos.y() + 15, pos.x() - 30, pos.y() + 25,
                   QPen(Qt::black, 2));
    scene->addLine(pos.x() + 20, pos.y() + 15, pos.x() + 30, pos.y() + 25,
                   QPen(Qt::black, 2));

    // Piernas
    double baseCuerpo = pos.y() + 40;

    scene->addLine(pos.x(), baseCuerpo, pos.x() - 10, ALTO_ESCENARIO,
                   QPen(Qt::black, 2));
    scene->addLine(pos.x(), baseCuerpo, pos.x() + 10, ALTO_ESCENARIO,
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

    double angulo = spinAngulo->value();
    double velocidad = spinVelocidad->value();

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
    ultimoTipoColision = "";
    contadorColisionesElasticas = 0;
    tiempoVueloProyectil = 0.0;

    timer->start(16);
    timerTimeout->start(TIMEOUT_SEGUNDOS * 1000);

    textoEstado->setPlainText("Proyectil en vuelo...");
    textoEstado->setDefaultTextColor(Qt::yellow);
    actualizarEstadoColision("EN VUELO");
}

void MainWindow::actualizarSimulacion()
{
    if (!proyectilActivo || !proyectilActivo->estaActiva()) {
        // Proyectil desactivado
        timer->stop();
        timerTimeout->stop();

        if (ultimoTipoColision != "RIVAL") {
            actualizarEstadoColision("DETENIDO");
        }

        limpiarProyectil();
        cambiarTurno();
        return;
    }

    proyectilActivo->actualizarMovimiento(DT);
    tiempoVueloProyectil += DT;
    verificarColisiones();

    // Verificar si esta practicamente detenido
    if (proyectilDetenido()) {
        timer->stop();
        timerTimeout->stop();
        actualizarEstadoColision("DETENIDO");
        limpiarProyectil();
        cambiarTurno();
        return;
    }

    if (proyectilActivo && proyectilActivo->estaActiva() && itemProyectil) {
        itemProyectil->setPos(proyectilActivo->getX(), proyectilActivo->getY());
    }
}

void MainWindow::verificarColisiones()
{
    if (!proyectilActivo || !motorColisiones) return;

    InfoColision info = motorColisiones->verificarColisiones(
        *proyectilActivo, muros, posRivalJ1, posRivalJ2, turnoActual
        );

    if (info.ocurrioColision) {
        if (info.tipoColision == "rival") {
            timer->stop();
            timerTimeout->stop();
            actualizarEstadoColision("¡IMPACTO!");
            mostrarVictoria(turnoActual);
        }
        else if (info.tipoColision == "muro") {
            int indiceMuro = info.indiceObjetoImpactado;
            if (indiceMuro >= 0 && indiceMuro < (int)muros.size()) {
                double danio = FACTOR_DANIO * info.momentoImpacto;
                muros[indiceMuro]->recibirDanio(danio);
                actualizarBarrasVida();
                actualizarEstadoColision("INELASTICA");
                ultimoTipoColision = "INELASTICA";
                contadorColisionesElasticas = 0;
            }
        }
        else if (info.tipoColision == "pared") {
            contadorColisionesElasticas++;
            actualizarEstadoColision("ELASTICA");
            ultimoTipoColision = "ELASTICA";
        }
    }
}

void MainWindow::verificarTimeout()
{
    // Se acabaron los 15 segundos
    if (proyectilEnMovimiento && timer->isActive()) {
        timer->stop();
        actualizarEstadoColision("TIMEOUT - Cambio de turno");
        textoEstado->setPlainText("Tiempo agotado");
        textoEstado->setDefaultTextColor(Qt::red);
        limpiarProyectil();
        cambiarTurno();
    }
}

bool MainWindow::proyectilDetenido()
{
    if (!proyectilActivo) return true;

    double velocidadActual = proyectilActivo->getVelocidadActual();

    // Considerar detenido si la velocidad es muy baja
    return velocidadActual < VELOCIDAD_MINIMA;
}

void MainWindow::actualizarEstadoColision(const QString &tipo)
{
    if (!labelEstadoColision) return;

    QString texto;
    QString estilo;

    if (tipo == "ELASTICA") {
        texto = QString("Jugador %1\n@ COLISION ELASTICA\nRebotes: %2")
                    .arg(turnoActual)
                    .arg(contadorColisionesElasticas);
        estilo = "background-color: #C8E6C9; color: #1B5E20; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else if (tipo == "INELASTICA") {
        texto = QString("Jugador %1\n# COLISION INELASTICA\n(con muro)").arg(turnoActual);
        estilo = "background-color: #FFE0B2; color: #E65100; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else if (tipo == "DETENIDO") {
        texto = QString("Jugador %1\nx DETENIDO\n(cambio de turno)").arg(turnoActual);
        estilo = "background-color: #E0E0E0; color: #424242; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else if (tipo == "¡IMPACTO!") {
        texto = QString("Jugador %1\n$ ¡IMPACTO!\n¡VICTORIA!").arg(turnoActual);
        estilo = "background-color: #FFCDD2; color: #B71C1C; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else if (tipo.contains("TIMEOUT")) {
        texto = QString("/ TIMEOUT\nTiempo agotado\n(15 segundos)");
        estilo = "background-color: #FFE082; color: #F57F17; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else if (tipo == "EN VUELO") {
        texto = QString("Jugador %1\n EN VUELO...").arg(turnoActual);
        estilo = "background-color: #BBDEFB; color: #0D47A1; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }
    else {
        texto = "Esperando disparo...";
        estilo = "background-color: #f0f0f0; color: #666; "
                 "padding: 10px; border-radius: 5px; font-size: 11px; font-weight: bold;";
    }

    labelEstadoColision->setText(texto);
    labelEstadoColision->setStyleSheet(estilo);
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
    contadorColisionesElasticas = 0;
    tiempoVueloProyectil = 0.0;
}

void MainWindow::cambiarTurno()
{
    turnoActual = (turnoActual == 1) ? 2 : 1;
    actualizarVisualizacion();
    textoEstado->setPlainText("Listo para disparar");
    textoEstado->setDefaultTextColor(Qt::yellow);
    actualizarEstadoColision(""); // Resetear
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
    double angulo = spinAngulo->value();
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
    labelAngulo->setText(QString("Angulo: %1°").arg(valor));
    actualizarAnguloCanon();
}

void MainWindow::actualizarInfoVelocidad(int valor)
{
    labelVelocidad->setText(QString("Velocidad: %1 m/s").arg(valor));
}

void MainWindow::resetearJuego()
{
    if (timer) {
        timer->stop();
    }
    if (timerTimeout) {
        timerTimeout->stop();
    }

    limpiarProyectil();

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
    ultimoTipoColision = "";
    contadorColisionesElasticas = 0;
    tiempoVueloProyectil = 0.0;

    sliderAngulo->setValue(45);
    sliderVelocidad->setValue(400);

    actualizarVisualizacion();
    actualizarEstadoColision("");
    textoEstado->setPlainText("Juego reiniciado");
    textoEstado->setDefaultTextColor(Qt::green);

    setFocus();
}

void MainWindow::mostrarVictoria(int jugadorGanador)
{
    timer->stop();
    timerTimeout->stop();
    juegoActivo = false;

    QString mensaje = QString("¡Jugador %1 ha ganado!").arg(jugadorGanador);
    textoEstado->setPlainText(mensaje);
    textoEstado->setDefaultTextColor(Qt::green);
    textoEstado->setFont(QFont("Arial", 14, QFont::Bold));

    QMessageBox::information(this, "¡Victoria!",
                             mensaje + "\n\nPresiona R para reiniciar.");

    setFocus();
}

// CONTROLES DE TECLADO
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!juegoActivo && event->key() != Qt::Key_R) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up:
        // Aumentar angulo +1°
        if (sliderAngulo->value() < 90) {
            sliderAngulo->setValue(sliderAngulo->value() + 1);
        }
        break;

    case Qt::Key_Down:
        // Disminuir angulo -1°
        if (sliderAngulo->value() > 0) {
            sliderAngulo->setValue(sliderAngulo->value() - 1);
        }
        break;

    case Qt::Key_Left:
        // Flecha izquierda: Disminuir velocidad -10 m/s
        if (sliderVelocidad->value() > 100) {
            int nuevoValor = sliderVelocidad->value() - 10;
            if (nuevoValor < 100) nuevoValor = 100;
            sliderVelocidad->setValue(nuevoValor);
        }
        break;

    case Qt::Key_Right:
        // Flecha derecha: Aumentar velocidad +10 m/s
        if (sliderVelocidad->value() < 800) {
            int nuevoValor = sliderVelocidad->value() + 10;
            if (nuevoValor > 800) nuevoValor = 800;
            sliderVelocidad->setValue(nuevoValor);
        }
        break;

    case Qt::Key_Space:
        // Disparar
        if (!proyectilEnMovimiento) {
            disparar();
        }
        break;

    case Qt::Key_R:
        // Reiniciar
        resetearJuego();
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}
