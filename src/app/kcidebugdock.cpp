#include "kcidebugdock.h"

kciDebugDock::kciDebugDock(QWidget *parent) :
    QDockWidget(parent)
{
    //Set ObjectName
    setObjectName("Debug Dock");

    //Set Style
    setContentsMargins(0,0,0,0);
    QPalette pal=this->palette();
    pal.setBrush(QPalette::Window, QBrush(QColor(83,83,83)));
    pal.setColor(QPalette::Base,QColor(0x35,0x35,0x35));
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    pal.setColor(QPalette::Button,QColor(83,83,83));
    pal.setColor(QPalette::Text,QColor(255,255,255));
    pal.setColor(QPalette::ButtonText,QColor(255,255,255));
    setPalette(pal);
    setWindowTitle(" " + tr("Debug") + " ");

    //New Central Widget
    CentralWidget=new kciDebugWidget(this);
    setWidget(CentralWidget);
}

kciDebugWidget::kciDebugWidget(QWidget *parent) :
    QWidget(parent)
{   
    m_parent=qobject_cast<kciDebugDock*>(parent);

    setContentsMargins(0,0,0,0);

    //Main Layout.
    MainWidgetLayout=new QVBoxLayout;
    MainWidgetLayout->setContentsMargins(0,0,0,0);
    MainWidgetLayout->setSpacing(0);
    setLayout(MainWidgetLayout);

    //Create Buttons.
    createToolBar();

    //Set Sub Layout.
    MainShownLayout=new QHBoxLayout;
    MainShownLayout->setContentsMargins(0,0,0,0);
    MainShownLayout->setSpacing(5);
    MainWidgetLayout->addLayout(MainShownLayout);

    //Add Widgets to Sub Layout.
    createControlButtons();
    createStackView();
    CombinePanelStack=new QVBoxLayout;
    CombinePanelStack->setContentsMargins(0,0,0,0);
    CombinePanelStack->setSpacing(5);
    CombinePanelStack->addLayout(ControlPanelM);
    CombinePanelStack->addLayout(stackMain,1);
    MainShownLayout->addLayout(CombinePanelStack);

    //Add Widget to GDB Layout.
    createGDBConversation();

    //Add Watch Widget.
    createWatchLayout();

    gdbInstance=NULL;
}

kciDebugWidget::~kciDebugWidget()
{
    MainWidgetLayout->deleteLater();
    MainShownLayout->deleteLater();
    CombinePanelStack->deleteLater();
    ControlPanel1->deleteLater();
    ControlPanel2->deleteLater();
    ControlPanelM->deleteLater();
    stackMain->deleteLater();
    WatchLayout->deleteLater();
    InputToGDB->deleteLater();
    GDBMainLayout->deleteLater();
}

void kciDebugWidget::createStackView()
{
    //Set Main Layout.
    stackMain=new QVBoxLayout;
    stackMain->setContentsMargins(0,0,0,0);
    stackMain->setSpacing(5);

    //Set Label.
    lblStackView=new QLabel(this);
    lblStackView->setText(" " + tr("Stack View"));
    stackMain->addWidget(lblStackView);

    //Set TreeView.
    trevwStackView=new QTreeView(this);
    trevwStackView->setHeaderHidden(true);
    trevwStackView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stackMain->addWidget(trevwStackView);
}

void kciDebugWidget::createWatchLayout()
{
    //Create Watch TreeView.
    WatchLayout=new QVBoxLayout;
    WatchLayout->setContentsMargins(0,0,0,0);
    WatchLayout->setSpacing(5);
    //Add Label Widget.
    lblLocalWatch=new QLabel(this);
    lblLocalWatch->setText(" " + tr("Local Watch"));
    WatchLayout->addWidget(lblLocalWatch);
    //Add TreeView
    localWatchView=new QTreeView(this);
    QPalette pal=localWatchView->palette();
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    localWatchView->setPalette(pal);
    localWatchView->setHeaderHidden(true);
    WatchLayout->addWidget(localWatchView);

    //Add New Label Widget.
    lblWatch=new QLabel(this);
    lblWatch->setText(" " + tr("Custom Watch"));
    WatchLayout->addWidget(lblWatch);
    //Add TreeView
    watchView=new QTreeView(this);
    watchView->setPalette(pal);
    watchView->setHeaderHidden(true);
    WatchLayout->addWidget(watchView);

    //Add to MainLayout.
    MainShownLayout->addLayout(WatchLayout,1);
}

void kciDebugWidget::createGDBConversation()
{
    GDBMainLayout=new QVBoxLayout;
    GDBMainLayout->setContentsMargins(0,0,0,0);
    GDBMainLayout->setSpacing(5);
    //Create Input Box.
    InputToGDB=new QHBoxLayout;
    InputToGDB->setContentsMargins(0,0,0,0);
    InputToGDB->setSpacing(2);
    QLabel *InputTag=new QLabel(this);
    InputTag->setText(tr("Send command to GDB:"));
    InputToGDB->addWidget(InputTag);

    GDBCmd=new QComboBox(this);
    GDBCmd->setEditable(true);
    connect(GDBCmd,SIGNAL(activated(QString)),
            this,SLOT(onGDBCmdEditFinished(QString)));

    InputToGDB->addWidget(GDBCmd,1);
    GDBMainLayout->addLayout(InputToGDB);
    GDBInfo=new kciPlainTextBrowser(this);
    GDBMainLayout->addWidget(GDBInfo,1);

    //Add To MainLayout.
    MainShownLayout->addLayout(GDBMainLayout,1);
}

void kciDebugWidget::createToolBar()
{
    QString tmpToolTip;
    //Init ToolBar.
    DebugToolBar=new QToolBar(this);
    DebugToolBar->setContentsMargins(0,0,0,0);

    //Set New ToolButton.
    //Set Debug Main Control Buttons.
    tblStartDebug=new QToolButton(this);
    tblStartDebug->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/StartDebug.png"));
    tblStartDebug->setFixedSize(26,26);
    tmpToolTip=tr("Start debugging.");
    tblStartDebug->setToolTip(tmpToolTip);
    connect(tblStartDebug,SIGNAL(clicked()),
            this,SLOT(onStartDebugButtonClicked()));

    DebugToolBar->addWidget(tblStartDebug);
    tblStopDebug=new QToolButton(this);
    tblStopDebug->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/StopDebug.png"));
    tblStopDebug->setFixedSize(26,26);
    DebugToolBar->addWidget(tblStopDebug);
    DebugToolBar->addSeparator();

    //Set Debug Control Buttons.
    tblRunToCursor=new QToolButton(this);
    tblRunToCursor->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/RunToCursor.png"));
    tblRunToCursor->setFixedSize(26,26);
    DebugToolBar->addWidget(tblRunToCursor);
    DebugToolBar->addSeparator();

    //Set Debug Watch Buttons.
    tblAddWatch=new QToolButton(this);
    tblAddWatch->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/AddWatch.png"));
    tblAddWatch->setFixedSize(26,26);
    DebugToolBar->addWidget(tblAddWatch);

    tblRemoveWatch=new QToolButton(this);
    tblRemoveWatch->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/RemoveWatch.png"));
    tblRemoveWatch->setFixedSize(26,26);
    DebugToolBar->addWidget(tblRemoveWatch);

    tblEditWatch=new QToolButton(this);
    tblEditWatch->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/ModifyWatch.png"));
    tblEditWatch->setFixedSize(26,26);
    DebugToolBar->addWidget(tblEditWatch);

    MainWidgetLayout->addWidget(DebugToolBar,1);
}

void kciDebugWidget::createControlButtons()
{
    //Control Panel Main Layout.
    ControlPanelM=new QHBoxLayout;
    ControlPanelM->setContentsMargins(0,0,0,0);
    ControlPanelM->setSpacing(0);

    //Control Panel Layouts 1.
    ControlPanel1=new QVBoxLayout;
    ControlPanel1->setContentsMargins(0,0,0,0);
    ControlPanel1->setSpacing(0);
    ControlPanelM->addLayout(ControlPanel1);

    //Set New ToolButton.
    tblNextStep=new QToolButton(this);
    tblNextStep->setText(QString(" " + tr("Next Step")));
    tblNextStep->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/NextStep.png"));
    tblNextStep->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblNextStep->setFixedSize(160,30);
    ControlPanel1->addWidget(tblNextStep);
    tblNextLine=new QToolButton(this);
    tblNextLine->setText(QString(" " + tr("Next Line")));
    tblNextLine->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/NextLine.png"));
    tblNextLine->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblNextLine->setFixedSize(160,30);
    ControlPanel1->addWidget(tblNextLine);
    ControlPanel1->addSpacing(3);
    tblContinue=new QToolButton(this);
    tblContinue->setText(QString(" " + tr("Continue")));
    tblContinue->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/Continue.png"));
    tblContinue->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblContinue->setFixedSize(160,30);
    ControlPanel1->addWidget(tblContinue);
    ControlPanel1->addStretch();

    //Control Panel Layouts 2.
    ControlPanel2=new QVBoxLayout;
    ControlPanel2->setContentsMargins(0,0,0,0);
    ControlPanel2->setSpacing(0);
    ControlPanelM->addLayout(ControlPanel2);

    tblNextInstruction=new QToolButton(this);
    tblNextInstruction->setText(QString(" " + tr("Next Instruction")));
    tblNextInstruction->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/JumpFunction.png"));
    tblNextInstruction->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblNextInstruction->setFixedSize(160,30);
    ControlPanel2->addWidget(tblNextInstruction);
    tblIntoInstruction=new QToolButton(this);
    tblIntoInstruction->setText(QString(" " + tr("Into Instruction")));
    tblIntoInstruction->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/GetIntoFuntion.png"));
    tblIntoInstruction->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblIntoInstruction->setFixedSize(160,30);
    ControlPanel2->addWidget(tblIntoInstruction);
    ControlPanel2->addSpacing(3);
    tblSkipInstruction=new QToolButton(this);
    tblSkipInstruction->setText(QString(" " + tr("Continue Instruction")));
    tblSkipInstruction->setIcon(QIcon(":/DebugToolBar/image/Debug Docks/GetOutOfFuntion.png"));
    tblSkipInstruction->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tblSkipInstruction->setFixedSize(160,30);
    ControlPanel2->addWidget(tblSkipInstruction);
    ControlPanel2->addStretch();

}

void kciDebugWidget::setDbgReceiver(dbgOutputReceiver *receiver)
{
    Q_ASSERT(receiver!=NULL);

    watchView->setModel(receiver->getWatchModel());
    localWatchView->setModel(receiver->getLocalVarModel());
    trevwStackView->setModel(receiver->getStackInfoModel());
    GDBInfo->setDocument(receiver->getTextStreamOutput());
}

void kciDebugWidget::connectGDB(gdb *instance)
{
    if(gdbInstance!=NULL)
    {
        connectionHandles.disConnectAll();
    }

    gdbInstance=instance;
    connectionHandles+=connect(tblNextLine,SIGNAL(clicked()),
                               gdbInstance,SLOT(execNext()));
    connectionHandles+=connect(tblNextStep,SIGNAL(clicked()),
                               gdbInstance,SLOT(execStepi()));
    connectionHandles+=connect(tblNextInstruction,SIGNAL(clicked()),
                               gdbInstance,SLOT(execStepi()));
    connectionHandles+=connect(tblContinue,SIGNAL(clicked()),
                               gdbInstance,SLOT(execContinue()));
    connectionHandles+=connect(tblStopDebug,SIGNAL(clicked()),
                               gdbInstance,SLOT(execAbort()));
    connectionHandles+=connect(tblSkipInstruction,SIGNAL(clicked()),
                               gdbInstance,SLOT(execReturn()));
}

void kciDebugWidget::onGDBCmdEditFinished(QString command)
{
    qDebug()<<command;
    gdbInstance->write(qPrintable(command+"\n"));
}

void kciDebugWidget::onStartDebugButtonClicked()
{
    if(gdbInstance == NULL)
    {
        emit m_parent->requireStartDebug();
    }

    Q_ASSERT(gdbInstance!=NULL);

    gdbInstance->execRun();
}

void kciDebugDock::setDbgReceiver(dbgOutputReceiver *receiver)
{
    Q_ASSERT(receiver!=NULL);
    CentralWidget->setDbgReceiver(receiver);
}

void kciDebugDock::setGdbInstance(gdb *instance)
{
    Q_ASSERT(instance!=NULL);
    CentralWidget->connectGDB(instance);
}