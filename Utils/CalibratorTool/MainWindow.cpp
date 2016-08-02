#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
  wrapper = new GroundTruthWrapper();
  QObject::connect(wrapper, SIGNAL(cameraImage(QImage,QString)), this, SLOT(updateCameraImage(QImage,QString)));
  QObject::connect(wrapper, SIGNAL(segmentedImage(QImage,QString)), this, SLOT(updateSegmentedImage(QImage,QString)));
  
  ui->setupUi(this);
  
  deactivateSegmentation();
}

void MainWindow::updateCameraImage(QImage img, QString name)
{
  if (img.isNull()) {
    return;
  }
  
  if(name.compare(QString("Camera 1")))
  {
    ui->label_3->setAlignment(Qt::AlignCenter);
    ui->label_3->setPixmap(QPixmap::fromImage(img).scaled(ui->label_3->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
  }
  else{
    ui->label_4->setAlignment(Qt::AlignCenter);
    ui->label_4->setPixmap(QPixmap::fromImage(img).scaled(ui->label_4->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
  }
}

void MainWindow::updateSegmentedImage(QImage img, QString name)
{
  if (img.isNull()) {
    return;
  }
  
  if(name.compare(QString("Camera 1")))
  {
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
  }
  else{
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_2->setPixmap(QPixmap::fromImage(img).scaled(ui->label_2->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
  }
}

MainWindow::~MainWindow()
{
  delete wrapper;
  delete ui;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
  wrapper->shouldStop = true;
  QWidget::closeEvent(event);
}

void MainWindow::on_pushButton_clicked()
{
  color = white;
  setSliders(true);
}

void MainWindow::on_pushButton_2_clicked()
{
  color = green;
  setSliders(false);
}

void MainWindow::on_pushButton_3_clicked()
{
  color = blue;
  setSliders(false);
}

void MainWindow::on_pushButton_4_clicked()
{
  color = red;
  setSliders(false);
}

void MainWindow::on_pushButton_5_clicked()
{
  color = orange;
  setSliders(false);
}

void MainWindow::on_pushButton_6_clicked()
{
  color = yellow;
  setSliders(false);
}

void MainWindow::on_pushButton_7_clicked()
{
  color = black;
  setSliders(false);
}

void MainWindow::on_pushButton_8_clicked()
{
  if (segmenting) {
    segmenting = false;
    ui->pushButton_8->setText("Segment");
    deactivateSegmentation();
    wrapper->saveColorCalibration();
    wrapper->setSegmentation(false);
  }
  else
  {
    segmenting = true;
    ui->pushButton_8->setText("Save Segmentation");
    activateSegmentation();
    wrapper->setSegmentation(true);
  }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
  ui->label_11->setText(QString::number(position));
  if(color == white)
    colorCalibration.whiteThreshold.minR = position;
  else
    colorCalibration.ranges[color].hue.min = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
  ui->label_12->setText(QString::number(position));
  if(color == white)
    colorCalibration.whiteThreshold.minB = position;
  else
    colorCalibration.ranges[color].hue.max = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::on_horizontalSlider_3_sliderMoved(int position)
{
  ui->label_13->setText(QString::number(position));
  if(color == white)
    colorCalibration.whiteThreshold.minRB = position;
  else
    colorCalibration.ranges[color].saturation.min = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::on_horizontalSlider_4_sliderMoved(int position)
{
  ui->label_14->setText(QString::number(position));
  colorCalibration.ranges[color].saturation.max = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::on_horizontalSlider_5_sliderMoved(int position)
{
  ui->label_15->setText(QString::number(position));
  colorCalibration.ranges[color].intensity.min = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::on_horizontalSlider_6_sliderMoved(int position)
{
  ui->label_16->setText(QString::number(position));
  colorCalibration.ranges[color].intensity.max = position;
  wrapper->setColorCalibration(colorCalibration);
}

void MainWindow::deactivateSegmentation()
{
  ui->label->setVisible(false);
  ui->label_2->setVisible(false);
  ui->pushButton->setEnabled(false);
  ui->pushButton->setVisible(false);
  ui->pushButton_2->setEnabled(false);
  ui->pushButton_2->setVisible(false);
  ui->pushButton_3->setEnabled(false);
  ui->pushButton_3->setVisible(false);
  ui->pushButton_4->setEnabled(false);
  ui->pushButton_4->setVisible(false);
  ui->pushButton_5->setEnabled(false);
  ui->pushButton_5->setVisible(false);
  ui->pushButton_6->setEnabled(false);
  ui->pushButton_6->setVisible(false);
  ui->pushButton_7->setEnabled(false);
  ui->pushButton_7->setVisible(false);
  
  ui->label_5->setVisible(false);
  ui->label_6->setVisible(false);
  ui->label_7->setVisible(false);
  ui->label_8->setVisible(false);
  ui->label_9->setVisible(false);
  ui->label_10->setVisible(false);
  ui->label_11->setVisible(false);
  ui->label_12->setVisible(false);
  ui->label_13->setVisible(false);
  ui->label_14->setVisible(false);
  ui->label_15->setVisible(false);
  ui->label_16->setVisible(false);
  
  ui->horizontalSlider->setEnabled(false);
  ui->horizontalSlider->setVisible(false);
  ui->horizontalSlider_2->setEnabled(false);
  ui->horizontalSlider_2->setVisible(false);
  ui->horizontalSlider_3->setEnabled(false);
  ui->horizontalSlider_3->setVisible(false);
  ui->horizontalSlider_4->setEnabled(false);
  ui->horizontalSlider_4->setVisible(false);
  ui->horizontalSlider_5->setEnabled(false);
  ui->horizontalSlider_5->setVisible(false);
  ui->horizontalSlider_6->setEnabled(false);
  ui->horizontalSlider_6->setVisible(false);
}

void MainWindow::activateSegmentation()
{
  ui->pushButton->setEnabled(true);
  ui->pushButton->setVisible(true);
  ui->pushButton_2->setEnabled(true);
  ui->pushButton_2->setVisible(true);
  ui->pushButton_3->setEnabled(true);
  ui->pushButton_3->setVisible(true);
  ui->pushButton_4->setEnabled(true);
  ui->pushButton_4->setVisible(true);
  ui->pushButton_5->setEnabled(true);
  ui->pushButton_5->setVisible(true);
  ui->pushButton_6->setEnabled(true);
  ui->pushButton_6->setVisible(true);
  ui->pushButton_7->setEnabled(true);
  ui->pushButton_7->setVisible(true);
  
  ui->label->setVisible(true);
  ui->label_2->setVisible(true);
}

void MainWindow::setSliders(bool white)
{
  colorCalibration = wrapper->getColorCalibration();
  
  if (white) {
    ui->label_5->setText("Min R");
    ui->label_6->setText("Min B");
    ui->label_7->setText("Min R+B");
    ui->label_5->setVisible(true);
    ui->label_6->setVisible(true);
    ui->label_7->setVisible(true);
    ui->label_8->setVisible(false);
    ui->label_9->setVisible(false);
    ui->label_10->setVisible(false);
    ui->label_11->setVisible(true);
    ui->label_12->setVisible(true);
    ui->label_13->setVisible(true);
    ui->label_14->setVisible(false);
    ui->label_15->setVisible(false);
    ui->label_16->setVisible(false);
    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider->setVisible(true);
    ui->horizontalSlider->setMaximum(255);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider_2->setEnabled(true);
    ui->horizontalSlider_2->setVisible(true);
    ui->horizontalSlider_2->setMaximum(255);
    ui->horizontalSlider_2->setMinimum(0);
    ui->horizontalSlider_3->setEnabled(true);
    ui->horizontalSlider_3->setVisible(true);
    ui->horizontalSlider_3->setMaximum(255);
    ui->horizontalSlider_3->setMinimum(0);
    
    ui->horizontalSlider_4->setEnabled(false);
    ui->horizontalSlider_4->setVisible(false);
    ui->horizontalSlider_5->setEnabled(false);
    ui->horizontalSlider_5->setVisible(false);
    ui->horizontalSlider_6->setEnabled(false);
    ui->horizontalSlider_6->setVisible(false);
    
    ui->horizontalSlider->setValue(colorCalibration.whiteThreshold.minR);
    ui->horizontalSlider_2->setValue(colorCalibration.whiteThreshold.minB);
    ui->horizontalSlider_3->setValue(colorCalibration.whiteThreshold.minRB);
    ui->label_11->setText(QString::number(ui->horizontalSlider->value()));
    ui->label_12->setText(QString::number(ui->horizontalSlider_2->value()));
    ui->label_13->setText(QString::number(ui->horizontalSlider_3->value()));
    
  }
  else
  {
    ui->label_5->setText("Min H");
    ui->label_6->setText("Max H");
    ui->label_7->setText("Min S");
    ui->label_8->setText("Max S");
    ui->label_9->setText("Min I");
    ui->label_10->setText("Max I");
    ui->label_5->setVisible(true);
    ui->label_6->setVisible(true);
    ui->label_7->setVisible(true);
    ui->label_8->setVisible(true);
    ui->label_9->setVisible(true);
    ui->label_10->setVisible(true);
    ui->label_11->setVisible(true);
    ui->label_12->setVisible(true);
    ui->label_13->setVisible(true);
    ui->label_14->setVisible(true);
    ui->label_15->setVisible(true);
    ui->label_16->setVisible(true);
    ui->horizontalSlider->setEnabled(true);
    ui->horizontalSlider->setVisible(true);
    ui->horizontalSlider->setMaximum(255);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider_2->setEnabled(true);
    ui->horizontalSlider_2->setVisible(true);
    ui->horizontalSlider_2->setMaximum(255);
    ui->horizontalSlider_2->setMinimum(0);
    ui->horizontalSlider_3->setEnabled(true);
    ui->horizontalSlider_3->setVisible(true);
    ui->horizontalSlider_3->setMaximum(255);
    ui->horizontalSlider_3->setMinimum(0);
    ui->horizontalSlider_4->setEnabled(true);
    ui->horizontalSlider_4->setVisible(true);
    ui->horizontalSlider_4->setMaximum(255);
    ui->horizontalSlider_4->setMinimum(0);
    ui->horizontalSlider_5->setEnabled(true);
    ui->horizontalSlider_5->setVisible(true);
    ui->horizontalSlider_5->setMaximum(255);
    ui->horizontalSlider_5->setMinimum(0);
    ui->horizontalSlider_6->setEnabled(true);
    ui->horizontalSlider_6->setVisible(true);
    ui->horizontalSlider_6->setMaximum(255);
    ui->horizontalSlider_6->setMinimum(0);
    
    ui->horizontalSlider->setValue(colorCalibration.ranges[color].hue.min);
    ui->horizontalSlider_2->setValue(colorCalibration.ranges[color].hue.max);
    ui->horizontalSlider_3->setValue(colorCalibration.ranges[color].saturation.min);
    ui->horizontalSlider_4->setValue(colorCalibration.ranges[color].saturation.max);
    ui->horizontalSlider_5->setValue(colorCalibration.ranges[color].intensity.min);
    ui->horizontalSlider_6->setValue(colorCalibration.ranges[color].intensity.max);
    ui->label_11->setText(QString::number(ui->horizontalSlider->value()));
    ui->label_12->setText(QString::number(ui->horizontalSlider_2->value()));
    ui->label_13->setText(QString::number(ui->horizontalSlider_3->value()));
    ui->label_14->setText(QString::number(ui->horizontalSlider_4->value()));
    ui->label_15->setText(QString::number(ui->horizontalSlider_5->value()));
    ui->label_16->setText(QString::number(ui->horizontalSlider_6->value()));
  }
}
