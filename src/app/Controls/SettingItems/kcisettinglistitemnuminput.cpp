#include "kcisettinglistitemnuminput.h"

kciSettingListItemNumInput::kciSettingListItemNumInput(QWidget *parent) :
    kciSettingListItemBase(parent)
{
    //New Layout Sets.
    MainLayout=new QHBoxLayout(this);
    MainLayout->setContentsMargins(0,0,0,0);
    MainLayout->setSpacing(0);
    setLayout(MainLayout);

    //Clear Item Value;
    value=0;

    //Set Caption.
    MainLayout->addSpacing(5);
    MainLayout->addWidget(Caption);
    MainLayout->addSpacing(3);

    //Set Value Displayer.
    ValueDisplayer=new QLabel(this);
    MainLayout->addWidget(ValueDisplayer);

    //Set Text Editor.
    ValueSpin=new QSpinBox(this);
    ValueSpin->setEnabled(false);
    ValueSpin->hide();
    connect(ValueSpin, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    MainLayout->addWidget(ValueSpin);

    ValueDial=new QDial(this);
    ValueDial->setEnabled(false);
    ValueDial->hide();
    ValueDial->setFixedWidth(ValueDial->height());
    connect(ValueDial, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    MainLayout->addWidget(ValueDial);

    ValueSlider=new QSlider(Qt::Horizontal,this);
    ValueSlider->setEnabled(false);
    ValueSlider->hide();
    connect(ValueSlider, SIGNAL(valueChanged(int)),
            this, SLOT(setValue(int)));
    MainLayout->addWidget(ValueSlider,1);
    MainLayout->addSpacing(5);
    MainLayout->addStretch();
}

int kciSettingListItemNumInput::getValue() const
{
    return value;
}

void kciSettingListItemNumInput::setValue(int NewValue)
{
    if(NewValue > MaxValue)
    {
        NewValue=MaxValue;
    }
    if(NewValue < MinValue)
    {
        NewValue=MinValue;
    }
    value = NewValue;
    ValueDisplayer->setText(QString::number(value));
    ValueSpin->setValue(value);
    ValueDial->setValue(value);
    ValueSlider->setValue(value);
}

void kciSettingListItemNumInput::mousePressEvent(QMouseEvent *e)
{
    ValueDisplayer->hide();
    ValueSpin->setEnabled(true);
    ValueSpin->show();
    ValueDial->setEnabled(true);
    ValueDial->show();
    ValueSlider->setEnabled(true);
    ValueSlider->show();
    e->accept();
}

int kciSettingListItemNumInput::getMaxValue() const
{
    return MaxValue;
}

void kciSettingListItemNumInput::setMaxValue(int NewValue)
{
    if(NewValue < MinValue)
    {
        NewValue = MinValue;
    }
    MaxValue = NewValue;
    ValueSpin->setMaximum(NewValue);
    ValueSlider->setMaximum(NewValue);
    ValueDial->setMaximum(NewValue);
}


int kciSettingListItemNumInput::getMinValue() const
{
    return MinValue;
}

void kciSettingListItemNumInput::setMinValue(int NewValue)
{
    if(NewValue > MaxValue)
    {
        NewValue = MaxValue;
    }
    MinValue = NewValue;
    ValueSpin->setMinimum(NewValue);
    ValueSlider->setMinimum(NewValue);
    ValueDial->setMinimum(NewValue);
}
