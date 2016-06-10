#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QTreeWidgetItem>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QGridLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QApplication>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

#include "GroundTruthWrapper.h"

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  public slots:
  //Display video frame in player UI
  void updateCameraImage(QImage img, QString name);
  
  void updateSegmentedImage(QImage img, QString name);
  
  void closeEvent(QCloseEvent *event);
  
private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_4_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_horizontalSlider_sliderMoved(int position);

  void on_horizontalSlider_2_sliderMoved(int position);

  void on_horizontalSlider_3_sliderMoved(int position);

  void on_horizontalSlider_4_sliderMoved(int position);

  void on_horizontalSlider_5_sliderMoved(int position);

  void on_horizontalSlider_6_sliderMoved(int position);

  void on_pushButton_8_clicked();

private:
  
  void setSliders(bool white);
  
  void deactivateSegmentation();
  
  void activateSegmentation();
  
  ColorCalibration colorCalibration;
  Color color;
  
  GroundTruthWrapper* wrapper;
  Ui::MainWindow *ui;
  
  bool segmenting;
};

