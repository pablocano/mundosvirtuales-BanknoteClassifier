/*
 * File:   ThresholdSelector.cpp
 * Author: marcel
 *
 * Created on October 16, 2013, 12:22 PM
 */

#include "ThresholdSelector.h"
#include "ColorCalibrationView.h"

#include <QHBoxLayout>

ThresholdSelector::ThresholdSelector(const QString& name, ColorCalibrationWidget* parent,
                                     int min, int max)
: QGroupBox(name, parent), parent(parent)
{
  slider = new QSlider(Qt::Orientation::Horizontal, this);
  slider->setMinimum(min);
  slider->setMaximum(max);
  slider->setTickPosition(QSlider::TicksBothSides);

  lineEdit = new QLineEdit(QString::number(min), this);
  lineEdit->setFixedWidth(40);
  lineEdit->setValidator(new QIntValidator(min, max, lineEdit));

  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
  connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(lineEditChanged(QString)));

  QHBoxLayout* layout = new QHBoxLayout(this);
  setLayout(layout);
  layout->addWidget(lineEdit);
  layout->addWidget(slider);
}

void ThresholdSelector::updateWidgets()
{
  if(parent->currentColor == white)
  {
    setEnabled(true);
    updateSlider(parent->colorCalibrationView.controller.colorCalibration.whiteThreshold);
  }
  else
    setEnabled(false);
}

void ThresholdSelector::setEnabled(bool value)
{
  slider->setEnabled(value);
  lineEdit->setEnabled(value);
}

void ThresholdSelector::updateColorCalibration(int value)
{
  if(parent->currentColor == white)
  {
    updateColorCalibration(value, parent->colorCalibrationView.controller.colorCalibration.whiteThreshold);
    parent->colorCalibrationView.controller.colorCalibrationChanged = true;
  }
}

void ThresholdSelector::sliderChanged(int value)
{
  lineEdit->setText(QString::number(value));
  updateColorCalibration(value);
}

void ThresholdSelector::lineEditChanged(QString value)
{
  slider->setValue(value.toInt());
}
