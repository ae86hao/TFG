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
** Clase console
**  Implementa una terminal virtual capaz de comunicarse con el puerto serie
**
*******************************************************************************************/

#ifndef CONSOLE_H
#define CONSOLE_H

//Se define el tamaño de la consola
#define ancho_consola 1000
#define alto_consola 160

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    //Señal que se activa cuando se vayan a mandar mensajes
    // a través del puerto serie
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent = 0);

    //Función que envía el mensaje a través del puerto serie
    void putData(const QByteArray &data);
    // Función para activar la opción de envío de ecos
    void setLocalEchoEnabled(bool set);

private:
    bool localEchoEnabled;

};

#endif // CONSOLE_H
