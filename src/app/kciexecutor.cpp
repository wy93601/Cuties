/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *      Wang Luming <wlm199558@126.com>
 *      Miyanaga Saki <tomguts@126.com>
 *      Zhang Jiayi <bf109g2@126.com>
 *
 *  This file is part of Kreogist-Cute-IDE.
 *
 *    Kreogist-Cute-IDE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cute-IDE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cute-IDE.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kciexecutor.h"

#ifdef Q_OS_WIN32
const QString console_runner_path("KciConsoleRunner.exe");
#else
const QString console_runner_path("KciConsoleRunner");
#endif

#ifdef Q_OS_UNIX
static const int terminalCount = 8;
static const Terminal knownTerminals[terminalCount] =
{
    {"xterm", "-e"},
    {"gnome-terminal", "-x"},
    {"konsole", "-e"},
    {"aterm", "-e"},
    {"Eterm", "-e"},
    {"rxvt", "-e"},
    {"urxvt", "-e"},
    {"xfce4-terminal", "-x"}
};
#else
static const int terminalCount = 1;
static const Terminal knownTerminals[terminalCount] =
{
    {"cmd", "/k"}
};
#endif

Terminal kciRunner::getDefaultTerminal()
{
    QSettings settings(kciGlobal::settingsFileName,QSettings::IniFormat);
    settings.beginGroup("DefaultTerminal");
    Terminal ret(
           settings.value("name", knownTerminals[0].terminal_name).toByteArray().constData(),
           settings.value("arg",knownTerminals[0].arg).toByteArray().constData());
    settings.endGroup();

    return ret;
}

QStringList kciExecutor::getSupportTerminalList()
{
    QStringList ret;
    for(int i=0;i<terminalCount;i++)
        ret<<knownTerminals[i].terminal_name;
    return ret;
}

void kciExecutor::setDefaultTerminal(const int& num)
{
    if(Q_LIKELY(num<terminalCount))
    {
        QSettings settings(kciGlobal::settingsFileName,QSettings::IniFormat);
        settings.beginGroup("DefaultTerminal");
        settings.setValue("name",knownTerminals[num].terminal_name);
        settings.value("arg",knownTerminals[num].arg);
        settings.endGroup();
    }
}

void kciRunner::run()
{
    if(p!=NULL)
    {
        p->lock.lockForRead();

        if(p->process!=NULL)
        {
            p->process->kill();
            p->process->waitForFinished();
            delete p->process;
        }

        if(p->enabledBackExec)
        {
            p->process=new QProcess;
            p->process->setReadChannelMode(QProcess::MergedChannels);
            connect(p->process,SIGNAL(readyRead()),
                    this,SLOT(onReadyRead()));
            p->process->start(p->path);
        }
        else
        {
            QStringList arg;
            #ifdef Q_OS_UNIX
                Terminal terminal=getDefaultTerminal();
                arg<<terminal.arg<<qApp->applicationDirPath()+'/'+console_runner_path<<p->path;
                p->process=new QProcess;
                qDebug()<<arg;
                p->process->startDetached(QLatin1String(terminal.terminal_name),arg);
            #else
                arg<<p->path;
                p->process->startDetached(qApp->applicationDirPath()+'/'+console_runner_path, arg);
            #endif
        }

        if(p->enabledAutoInput)
            p->process->write(p->m_input);

        p->lock.unlock();
    }
    exec();
}

void kciRunner::onReadyRead()
{
    if(p!=NULL)
    {
        p->output_lock.lockForWrite();

        p->user_output+=p->process->readAll();

        p->output_lock.unlock();
    }
}

kciExecutor::kciExecutor(QObject *parent) :
    QObject(parent)
{
    enabledBackExec=false;
    process=NULL;
    worker=NULL;
}

kciExecutor::~kciExecutor()
{
    if(process!=NULL)
    {
        process->kill();
        process->waitForFinished();
        delete process;
    }

    if(worker!=NULL)
    {
        worker->quit();
        worker->wait();
        delete worker;
    }
}

void kciExecutor::setTestCase(const QByteArray &input, const QByteArray &output)
{
    m_input=input;
    m_output=output;
    worker=NULL;
    process=NULL;
}

bool kciExecutor::getBackgroundExec() const
{
    return enabledBackExec;
}

void kciExecutor::setBackgroundExec(bool value)
{
    lock.lockForWrite();
    enabledBackExec = value;
    lock.unlock();
}

bool kciExecutor::isEnabledAutoInput() const
{
    return enabledAutoInput;
}

void kciExecutor::setEnabledAutoInput(bool value)
{
    lock.lockForWrite();
    enabledAutoInput = value;
    lock.unlock();
}

void kciExecutor::exec(const QString &programPath)
{
    lock.lockForWrite();
    path=programPath;

    if(worker!=NULL)
    {
        worker->quit();
        worker->wait();
        delete worker;
    }

    worker=new kciRunner;
    worker->p=this;
    worker->start();

    lock.unlock();
}

QByteArray kciExecutor::getUserOutput()
{
    output_lock.lockForRead();
    QByteArray ret = user_output;
    output_lock.unlock();
    return ret;
}