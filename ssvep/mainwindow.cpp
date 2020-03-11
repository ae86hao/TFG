/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/
/******************************************************************************************
**  Ana Isabel Sotomayor Romillo
**
**  Trabajo de Fin de Máster:
**  Control adaptado al sujeto de una interfaz cerebro-máquina con señalización mixta
**
**  Máster en Ingeniería de Telecomunicaciones
**
** ----------------------------------------------
**
** MainWindow
**  Interfaz Gráfica de Usuario. Aplicación que se comporta como Interfaz Cerebro -
**  Máquina, comunicándose tanto con el casco Emotiv como con Arduino.
**
*******************************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "deviceDialog.h"
#include "mythread.h"

#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QThread>
#include <QtGlobal>
#include <QElapsedTimer>
#include <QTime>
#include <QProcess>

//La variable idCola se recibe desde el Main
extern int idCola;

//Variables globales para ser leídas desde el hilo
int frecuencia1_int;
int frecuencia2_int;

int frecuencia4_int;
int frecuencia5_int;


////////////////////////////////////////////////////////////////////////////////
/// Ventana Principal: Se crea MainWindow
////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ///////////////////////
    /// Inicialización de variables
    ///////////////////////

    frecuencia1_int = 0;
    frecuencia2_int = 0;

    frecuencia4_int = 0;
    frecuencia5_int = 0;


    contador_inicio=0;

    cadena_speller=" ";
    estado_NU=0;
    estado_UE=0;
    existe_usuario=false;
    contador12luces=0;
    numero_fallos=0;
    executable=ARCHIVO_EJECUCIONGTEC;
    numCanales=16;


    // Se crea la interfaz gráfica en el Main Window
    ui->setupUi(this);

    //Se puede seleccionar un tamaño fijo o mostrar a pantalla completa
    setFixedSize(ancho_pantalla, alto_pantalla);
    //showFullScreen();


    // Se crea la consola que irá mostrando el proceso y actuará como terminal.
    console = new Console(this);
    console->setEnabled(true);

    //Se fija como una ventana en el centro
    //Fijo ancho y alto
    console->setFixedSize(ancho_consola+100,alto_consola);
    //Lo centro en la ventana
    console->move(ancho_pantalla/2-ancho_consola/2-100, alto_pantalla/2-90);


    //Llamada a función de configuración inicial de ventana: Se esconden los estímulos
    setInicio();


    // Habilitación del puerto serie para comunicación con Arduino
    serial = new QSerialPort(this);
    settings = new SettingsDialog;
    device = new DeviceDialog;


    // Configuración inicial de los botones superiores
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->actionDevice->setEnabled(true);

    // Configuración de la barra de estado
    status = new QLabel;
    ui->statusBar->addWidget(status);


    //Función que crea las conexiones SIGNALS & SLOTS
    initActionsConnections();
}

////////////////////////////////////////////////////////////////////////////////
/// Función ~MainWindow
///     Encargada de borrar la GUI, así como todos los objetos heredados de
///     la misma
////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete settings;
    delete device;
    delete ui;
}

bool MainWindow::instanceFlag = false;
MainWindow* MainWindow::myinstance = NULL;

////////////////////////////////////////////////////////////////////////////////
/// Función getInstance()
///     Sirve para obtener una instancia de la ventana MainWindow
////////////////////////////////////////////////////////////////////////////////
MainWindow* MainWindow::getInstance()
  {
    static QMutex mutex;
    if (!myinstance){
      mutex.lock();
      if (!myinstance){
        myinstance = new MainWindow;
        instanceFlag = true;
      }
      mutex.unlock();
    }

    return myinstance;
  }


////////////////////////////////////////////////////////////////////////////////
/// Función showStatusMessage(const QString &message)
///     Al llamar a la función muestra el mensaje que recibe como parámetro
///     Sirve para mostrar la conexión al puerto serie
////////////////////////////////////////////////////////////////////////////////
void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

////////////////////////////////////////////////////////////////////////////////
/// Función openSerialPort()
///     Se ejecuta al pulsar el botón de conexión.
///     Se configuran los parámetros de configuración del puerto serie.
////////////////////////////////////////////////////////////////////////////////

void MainWindow::openSerialPort()
{
    MainWindow::chooseDevice();

    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        console->setEnabled(true);
        console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("ERROR al abrir el puerto serie"));
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Función closeSerialPort()
///     Se ejecuta al pulsar el botón de Desconectar Puerto Serie
////////////////////////////////////////////////////////////////////////////////
void MainWindow::closeSerialPort()
{
    QString stop = "66";

    emit console->getData(stop.toLocal8Bit());

    sleep(2);

    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

////////////////////////////////////////////////////////////////////////////////
/// Función chooseDevice()
///     Se ejecuta al pulsar el botón de dispositivo.
///     Se escoge uno de los dispositivos disponibles para utilizar.
////////////////////////////////////////////////////////////////////////////////
void MainWindow::chooseDevice()
{

    QString d = device->getDevice();

    if (QString::compare(d, "EMOTIV", Qt::CaseInsensitive) == 0){
        executable = ARCHIVO_EJECUCIONEMOTIV;
        numCanales = "14";
    }

}

////////////////////////////////////////////////////////////////////////////////
/// Función about()
///     Se ejecuta al pulsar el botón About, muestra información de la GUI
////////////////////////////////////////////////////////////////////////////////
void MainWindow::about()
{
    QMessageBox::about(this, tr("Interfaz Cerebro Máquina"),
                       tr("Este BCI permite la comunicación entre personas a través "
                          "de la actividad cerebral, habilitando un medio efectivo "
                          "de interrelación en personas con alguna discapacidad."));
}

////////////////////////////////////////////////////////////////////////////////
/// Función writeData(const QByteArray &data)
///     Envío de datos al puerto serie
///     Envía al puerto serie el QByteArray que recibe como parametro
////////////////////////////////////////////////////////////////////////////////
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
    //console->appendPlainText("Se han mandado datos al puerto serie");
}

////////////////////////////////////////////////////////////////////////////////
/// Función readData()
///     Recepción de datos desde el puerto serie. Se muestran en la consola
////////////////////////////////////////////////////////////////////////////////
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    console->putData(data);
    //console->appendPlainText("Se han leido datos del puerto serie");
}

////////////////////////////////////////////////////////////////////////////////
/// Función handleError(QSerialPort::SerialPortError error)
///     En caso de error, se cierra el puerto serie
////////////////////////////////////////////////////////////////////////////////
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

////////////////////////////////////////////////////////////////////////////////
///  Función initActionsConnections()
///     Mecanismo de conexiones SIGNALS & SLOTS. Conexiones necesarias para
///     el funcionamiento del programa
////////////////////////////////////////////////////////////////////////////////
void MainWindow::initActionsConnections()
{
    //Conexión de los botones de acción
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionDevice, SIGNAL(triggered()), device, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // Si ocurre un error en la apertura del puerto serie, notificar en la ventana principal
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

    // Si el puerto serie está preparado para ser leído, llama a la UI para que le lea
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    //Desde la consola mandar datos al puerto serie
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    //Conexion de estados. Cuando se emite la señal Signal_EstimuloX, se activan
    // los procedimientos de detección de estímulos
    connect(this, SIGNAL(Signal_Estimulo1()), this, SLOT(Estimulo1Detectado()));
    connect(this, SIGNAL(Signal_Estimulo2()), this, SLOT(Estimulo2Detectado()));

    connect(this, SIGNAL(Signal_Estimulo4()), this, SLOT(Estimulo4Detectado()));
    connect(this, SIGNAL(Signal_Estimulo5()), this, SLOT(Estimulo5Detectado()));



    //Se activan también estos mismos procedimientos si se pulsan los botones de ayuda
    connect(ui->pushButton_1, SIGNAL(clicked()), this, SLOT(Estimulo1Detectado()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(Estimulo2Detectado()));

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(Estimulo4Detectado()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(Estimulo5Detectado()));


}

////////////////////////////////////////////////////////////////////////////////
/// Función setInicio()
///     Coloca los botones en sus posiciones, y esconde los estímulos hasta que
///     se realice la configuración inicial.
////////////////////////////////////////////////////////////////////////////////
void MainWindow::setInicio()
{
    estado =0;
    contador_inicio=0;

    //Se muestra el texto de bienvenida
    ui->Bienvenido->move(ancho_pantalla/2-ui->Bienvenido->width()/2, alto_pantalla/4);
    ui->InicieSesion->move(ancho_pantalla/2-ui->InicieSesion->width()/2, alto_pantalla/3);

    //Se configuran los botones de Nuevo Usuario y Usuario Regustrado
    ui->Boton_NuevoUsu->move(ancho_pantalla/2-ui->Boton_NuevoUsu->width()/2-distancia_botones/2, alto_pantalla/2);
    ui->Boton_RegUsu->move(ancho_pantalla/2-ui->Boton_RegUsu->width()/2+distancia_botones/2, alto_pantalla/2);

    //Todo lo demás se esconde hasta que se pulse uno de los dos botones
    ui->boton_inicio->hide();
    ui->label_InfoNuevoUsu->hide();
    ui->label_introducenombre->hide();
    ui->line_NombreUsu->hide();
    QFont f( "DejaVu Serif", 36, QFont::Bold);
    console->setFont(f);
    console->hide();
    ui->estimulo1->hide();
    ui->estimulo2->hide();

    ui->estimulo4->hide();
    ui->estimulo5->hide();

    ui->label_arrow->hide();
}

////////////////////////////////////////////////////////////////////////////////
/// Función on_Boton_NuevoUsu_clicked(): Creación de un nuevo Usuario
///     Solicita un nombre de Usuario, le asigna un número de Usuario y
///     lo almacena en el fichero de registro de usuarios
////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_Boton_NuevoUsu_clicked()
{
    int i;
    ui->label_introducenombre->hide();
    // estado_NU será igual a 1 al pulsar el botón por segunda vez.
    // En este caso se ha recibido ya el nombre de usuario, y se registra

    if (estado_NU==1){ //Almacenar Usuario

        QFile file_regusus(RUTA_RegUsus);

        if (!file_regusus.open(QIODevice::ReadWrite | QIODevice::Text)){
            ui->label_introducenombre->show();
            ui->label_introducenombre->setText("ERROR! No se encuentra el registro");
            return;
        }

        nombre_usuario=ui->line_NombreUsu->text();
        nombre_usuario += "\n";
        //Se lee hasta el final el registro de usuarios
        for (i = 1; !file_regusus.atEnd(); i++){
            numero_usuario=file_regusus.readLine();
            if (nombre_usuario.compare(numero_usuario) == 0){
                ui->label_introducenombre->show();
                ui->label_introducenombre->setText("ERROR! Nombre ya usado");
                return;
            }
        }
        numero_usuario=QString::number(i);

        ui->label_introducenombre->show();

        QString auxi ="Eres el usuario: " + numero_usuario + " Con nombre " + nombre_usuario;
        ui->label_introducenombre->setText(auxi);
        ui->label_introducenombre->move(ui->Boton_NuevoUsu->x(), ui->Boton_NuevoUsu->y()+100);
        ui->label_introducenombre->setFixedSize(300,50);

        file_regusus.close();

        //Se vuelve a abrir el fichero en modo Append (añadir al final del fichero)
        if (!file_regusus.open(QIODevice::Append | QIODevice::Text))
            return;

        QTextStream out(&file_regusus);
        out << nombre_usuario;
        file_regusus.close();

        //Se crea una carpeta para el usuario
        QString dir = QString("/home/gnb/Hao_Zhu/ejecucion/datos/Usuario_%1").arg(numero_usuario);
        if (!QDir(dir).exists())
            QDir().mkdir(dir);

        //Una vez se almacena el usuario, se muestra el botón de grabar Baseline
        ui->boton_inicio->show();
        ui->boton_inicio->setText("Pulse aquí para grabar baseline");
        ui->boton_inicio->setFixedSize(ancho_consola, alto_consola-80);
        ui->boton_inicio->move(ancho_pantalla/2-ancho_consola/2, alto_pantalla*3/4);

    }
    else{
        //La primera vez que se pulsa el botón de Nuevo Usuario, se solicita un nombre
        ui->label_InfoNuevoUsu->show();
        ui->line_NombreUsu->show();
        ui->label_InfoNuevoUsu->move(ui->Boton_NuevoUsu->x(), ui->Boton_NuevoUsu->y()+50);
        ui->line_NombreUsu->move(ui->label_InfoNuevoUsu->x()+ui->label_InfoNuevoUsu->width() +20, ui->Boton_NuevoUsu->y()+50);
        estado_NU=1;
        ui->Boton_NuevoUsu->setText("Crear Usuario");
    }

}

////////////////////////////////////////////////////////////////////////////////
/// Función on_Boton_RegUsu_clicked(): Validación de Usuario Registrado
///     Solicita un nombre de Usuario, se verifica que se encuentre en el
///     registro y se busca su número de usuario para establecer sus frecuencias
////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_Boton_RegUsu_clicked()
{
    int i;
    ui->label_introducenombre->hide();
    // estado_UE será igual a 1 al pulsar el botón por segunda vez.
    // En este caso se ha recibido ya el nombre de usuario, y se valida
    if (estado_UE==1){ //Validar Usuario

        QFile file_regusus(RUTA_RegUsus);

        if (!file_regusus.open(QIODevice::ReadWrite | QIODevice::Text)){
            ui->label_introducenombre->show();
            ui->label_introducenombre->setText("ERROR! No se encuentra el registro");
            return;
        }

        nombre_usuario=ui->line_NombreUsu->text();
        nombre_usuario += "\n";
        existe_usuario = false;

        for (i = 1; !file_regusus.atEnd(); i++){
            numero_usuario=file_regusus.readLine();
            if (nombre_usuario.compare(numero_usuario) == 0){
                existe_usuario = true;
                break;
            }
        }

        if (existe_usuario==true){

            numero_usuario=QString::number(i);
            ui->label_introducenombre->show();
            nombre_usuario="Eres el usuario: " + numero_usuario + " Con nombre " + nombre_usuario;
            ui->label_introducenombre->setText(nombre_usuario);
            ui->label_introducenombre->move(ui->Boton_RegUsu->x(), ui->Boton_RegUsu->y()+100);
            ui->label_introducenombre->setFixedSize(300,50);

            contador_inicio = 3;
            ui->boton_inicio->show();
            ui->boton_inicio->setText("Pulse aquí para comenzar el programa");
            ui->boton_inicio->setFixedSize(ancho_consola, alto_consola-80);
            ui->boton_inicio->move(ancho_pantalla/2-ancho_consola/2, alto_pantalla*3/4);

        }
        else
            ui->label_InfoNuevoUsu->setText("Usuario no existe.");
    }

    ui->label_InfoNuevoUsu->show();
    ui->line_NombreUsu->show();
    ui->label_InfoNuevoUsu->move(ui->Boton_RegUsu->x(), ui->Boton_RegUsu->y()+50);
    ui->line_NombreUsu->move(ui->label_InfoNuevoUsu->x()+ui->label_InfoNuevoUsu->width() +20, ui->Boton_RegUsu->y()+50);
    estado_UE=1;
    ui->Boton_RegUsu->setText("Validar Usuario");


}

////////////////////////////////////////////////////////////////////////////////
/// Función on_boton_inicio_clicked():
///     La primera vez que se pulsa graba el baseline del usuario
///     La segunda, comienza el programa con normalidad
///     La tercera, finaliza el programa
////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_boton_inicio_clicked()
{

    //Se configura la pantalla vacía y en negro
    ui->Bienvenido->hide();
    ui->InicieSesion->hide();
    ui->Boton_NuevoUsu->hide();
    ui->Boton_RegUsu->hide();
    ui->label_introducenombre->hide();
    ui->label_InfoNuevoUsu->hide();
    ui->line_NombreUsu->hide();
    ui->boton_inicio->hide();

    // La primera vez que se pulsa, graba el baseline del usuario
    switch(contador_inicio){
    case 0:
        // Grabar y Guardar Baseline
        accion = "1"; //1 corresponde a grabar baseline
        console->hide();
        break;
    case 1:
    {
        // Barrido de frecuencias

        accion = "4"; //4 corresponde a barrido de frecuencias
        break;
    }
    case 2:
    {
        // Seleccion de frecuencias optimas

        accion = "3"; //3 corresponde a seleccion de frecuencias
        break;
    }
    case 3:
    {
        ui->boton_inicio->setFixedSize(200, 200);
        ui->boton_inicio->move(2*ancho_pantalla/3 + margen_lateral + 100, alto_pantalla/2 + margen_superior/2 + 100);
        ui->boton_inicio->setText("finalizar");
        ui->boton_inicio->show();

        // Establecer posiciones
        setPosiciones();
        console->show();

        //Estado 10: Menú inicial
        estado = 10;
        checkEstados(estado);


        //Se lee el fichero de mejores frecuencias del sujeto
        QString rutaMejoresFrecuencias = "/home/gnb/Hao_Zhu/ejecucion/datos/Usuario_" + numero_usuario + "/configAnalisis.cfg";
        QFile file(rutaMejoresFrecuencias);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        console->appendPlainText("El fichero no se ha abierto");
        }

        while (!file.atEnd()) {

            //Se leen las frecuencias y se guardan como QString
            frecuencias_completas=file.readLine();
            lista_frecuencias= frecuencias_completas.split(" ", QString::SkipEmptyParts);
            frecuencia1=lista_frecuencias.at(0);
            frecuencia2=lista_frecuencias.at(1);

            frecuencia4=lista_frecuencias.at(2);
            frecuencia5=lista_frecuencias.at(3);


        }

        //Se envían las frecuencias a arduino para configurarlas. Se añade el "99" inicial de flag para el puerto serie, y un "1" de Configuración

        frecuencias_completas =  "99 " + frecuencia1 + " " + frecuencia2 + " " + frecuencia4 + " " + frecuencia5;
        emit console->getData(frecuencias_completas.toLocal8Bit());


        frecuencia1_int=frecuencia1.toInt();
        frecuencia2_int=frecuencia2.toInt();

        frecuencia4_int=frecuencia4.toInt();
        frecuencia5_int=frecuencia5.toInt();

        accion = "2"; //2 corresponde a ejecucion emotiv
        break;
    }
    default:
        this->close();

    }
    contador_inicio++;

    QProcess *proceso = new QProcess(this);
    QStringList argumentos;
    QString idCola_string = QString::number(idCola);
    argumentos << numero_usuario << accion << idCola_string << numCanales;
    proceso->start(executable, argumentos);
    proceso->waitForStarted();
}


////////////////////////////////////////////////////////////////////////////////
/// Función setPosiciones()
///     Establece las posiciones de los estímulos, después de grabar el Baseline
////////////////////////////////////////////////////////////////////////////////
void MainWindow::setPosiciones()
{

//Mostrar el menú y los estímulos

    console->show();
    ui->estimulo1->show();
    ui->estimulo2->show();

    ui->estimulo4->show();
    ui->estimulo5->show();


// Centrar estímulos

    ui->estimulo1->setFixedSize(ancho_estimulo, alto_estimulo);
    ui->estimulo2->setFixedSize(ancho_estimulo, alto_estimulo);

    ui->estimulo4->setFixedSize(ancho_estimulo, alto_estimulo);
    ui->estimulo5->setFixedSize(ancho_estimulo, alto_estimulo);


    ui->estimulo1->move(margen_lateral,margen_superior);
    ui->estimulo2->move(ancho_pantalla/3 + margen_lateral, margen_superior);

    ui->estimulo4->move(margen_lateral,alto_pantalla/2 + margen_superior/2);
    ui->estimulo5->move(ancho_pantalla/3 + margen_lateral,alto_pantalla/2 + margen_superior/2);


//Boton centrado en la esquina superior izquierda de cada estimulo
      ui->pushButton_1->move(0,0);
      ui->pushButton_2->move(0,0);

      ui->pushButton_4->move(0,0);
      ui->pushButton_5->move(0,0);



//Tamaño de texto grande
      QFont f( "DejaVu Serif", 36, QFont::Bold);
      for(int i = 0; i < num_estimulos; i++)
        ui->labels->at(i)->setFont(f);

      console->setFont(f);
      console->clear();

//Texto centrado en cada estímulo
      for(int i = 0; i < num_estimulos; i++){
        ui->labels->at(i)->move(0,0);
        ui->labels->at(i)->setFixedSize(ancho_estimulo, alto_estimulo);
      }

}

////////////////////////////////////////////////////////////////////////////////////////////
/// AQUI COMIENZA LA MÁQUINA DE ESTADOS
///////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// Función setEstado0
///     Estado 0 corresponde con el Menú Inicial de la GUI
////////////////////////////////////////////////////////////////////////////////
void MainWindow::setEstado0()
{
    cadena_speller="";

    estado = 2;
    checkEstados(estado);

}


///////////////////////////////////////////////////////////////////////////////////
/// Función checkEstados(int estados)
///     Controlador de Estados. Dependiendo del estado que recibe como parámetro
///     llama a la función correspondiente
///////////////////////////////////////////////////////////////////////////////////

void MainWindow::checkEstados(int estados)
{
  int testados, auxi;
  QList<int> secuencia;
  
  testados = estados;

  if (estados == 10){
    setEstado0();
    return;
  }

  auxi = testados % 10;
  while(auxi != 0){
    secuencia.append(auxi);
    testados /= 10;
    auxi = testados % 10;
  }

  if (!secuencia.isEmpty() && secuencia.last() == 2){
      showSpeller(secuencia, QString(CADENA));
  }
}

///////////////////////////////////////////////////////////////////////////////////
/// Función showSpeller()
///     C
///     l
///////////////////////////////////////////////////////////////////////////////////
void MainWindow::showSpeller(QList<int> sec, QString cad){
  int len, stimLen, target;
  QStringList cadList;
  
  while(!sec.isEmpty()){
    sec.removeLast();
    cadList.clear();
    len = cad.length();
    stimLen = len / num_estimulos;
    if (len == 0){
        estado = 2;
        checkEstados(estado);
    }
    else if (len == 1){
      cadena_speller += cad;
      console->appendPlainText(cadena_speller);
      estado = 2;
      checkEstados(estado);
      return;
    }
    else if (stimLen == 0){
      for(int j = 0; j < len; j++)
        cadList.append(QString(cad.at(j)));
      if (!sec.isEmpty()){
        target = sec.last();
        if (cadList.size() < target){
            estado = 2;
            checkEstados(estado);
        }
      }
    }
    else{

      for(int j = 0; j < num_estimulos - 1; j++){
        cadList.append(cad.left(stimLen));
        cad.remove(0,stimLen);
      }
      cadList.append(cad);

      if (!sec.isEmpty()){
        target = sec.last();
        cad = cadList.at(target - 1);
      }
    }
  }

  for(int j = 0; j < cadList.size(); j++)
      ui->labels->at(j)->setText(cadList.at(j));
  for(int j = cadList.size(); j < num_estimulos; j++)
      ui->labels->at(j)->setText("");
  
}

///////////////////////////////////////////////////////////////////////////////////////////
/// SLOTS: ESTÍMULOS DETECTADOS
///////////////////////////////////////////////////////////////////////////////////////////


/// ///////////////////////////////////////////////////////////////////////////////
/// Función Estimulo1Detectado()
///     En función del estado en el que se encuentre cuando se detecta el esntímulo
///     se invocará al estado correspondiente de la máquina de estados.
///     En esta función se selecciona la opción contenida en el estímulo superior
///     izquierdo.
///////////////////////////////////////////////////////////////////////////////////
void MainWindow::Estimulo1Detectado()
{
  estado = estado*10 + 1;

  checkEstados(estado);
}


/// ///////////////////////////////////////////////////////////////////////////////
/// Función Estimulo2Detectado()
///     En función del estado en el que se encuentre cuando se detecta el estímulo
///     se invocará al estado correspondiente de la máquina de estados.
///     En esta función se selecciona la opción contenida en el estímulo superior
///     central.
///////////////////////////////////////////////////////////////////////////////////
void MainWindow::Estimulo2Detectado(){
  estado = estado*10 + 2;

  checkEstados(estado);
}



/// ///////////////////////////////////////////////////////////////////////////////
/// Función Estimulo4Detectado()
///     En función del estado en el que se encuentre cuando se detecta el estímulo
///     se invocará al estado correspondiente de la máquina de estados.
///     En esta función se selecciona la opción contenida en el estímulo inferior
///     izquierdo.
///////////////////////////////////////////////////////////////////////////////////

void MainWindow::Estimulo4Detectado(){
  estado = estado*10 + 3;

  checkEstados(estado);
}

void MainWindow::Estimulo5Detectado(){
  estado = estado*10 + 4;

  checkEstados(estado);
}

void MainWindow::ConfigLEDDetectado(QString freq){
    console->hide();
    emit console->getData(freq.toLocal8Bit());
}

void MainWindow::DescansoDetectado(int i, int j){
    console->show();
    console->appendPlainText(QString("Descanso(%1/%2)").arg(i).arg(j));
}

void MainWindow::SetStimPosDetectado(int p){
    QPixmap pixmap("/home/gnb/Hao_Zhu/ssvep/images/arrow.jpg");
    QMatrix rm;

    switch(p){
    case TOPL:
        rm.rotate(135);
        break;
    case TOPR:
        rm.rotate(225);
        break;
    case BOTL:
        rm.rotate(45);
        break;
    case BOTR:
        rm.rotate(315);
        break;
    }

    pixmap = pixmap.transformed(rm);
    ui->label_arrow->setPixmap(pixmap);
    ui->label_arrow->show();
}

void MainWindow::FinSetStimPosDetectado(){
    ui->label_arrow->hide();
}

void MainWindow::FinBaselineDetectado(){
    ui->boton_inicio->show();
    ui->boton_inicio->setText("Pulse aquí para el barrido de frecuencias");
}

void MainWindow::FinBarridoDetectado(){
    ui->boton_inicio->show();
    ui->boton_inicio->setText("Pulse aquí para el algoritmo ACL");
}

void MainWindow::FinSelectFreqsDetectado(){
    ui->boton_inicio->show();
    ui->boton_inicio->setText("Pulse aquí para comenzar speller");
}

