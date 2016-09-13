//
//  ListViewsDockWidget.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 13-09-16.
//
//

#pragma once 
#include <QDockWidget>
#include <QSet>
#include <QHash>
#include <QTreeWidgetItem>

#include "CalibratorTool.h"

class ListViewsDockWidget : public QDockWidget
{
  Q_OBJECT
  
public:
  ListViewsDockWidget(QWidget* parent);
  virtual ~ListViewsDockWidget();
  
  void registerObject(CalibratorTool::Object* object, const CalibratorTool::Object* parent, int flags);
  void unregisterAllObjects();
  bool unregisterObject(const CalibratorTool::Object* object);
  CalibratorTool::Object* resolveObject(const QString& fullName, int kind);
  CalibratorTool::Object* resolveObject(const CalibratorTool::Object* parent, const QVector<QString>& parts, int kind);
  int getObjectChildCount(const CalibratorTool::Object* object);
  CalibratorTool::Object* getObjectChild(const CalibratorTool::Object* object, int index);
  
  bool activateFirstObject();
  bool activateObject(const CalibratorTool::Object* object);
  bool setOpened(const CalibratorTool::Object* object, bool opened);
  bool setActive(const CalibratorTool::Object* object, bool active);
  
  QAction* toggleViewAction() const;
  
signals:
  void activatedObject(const QString& fullName, CalibratorTool::Object* object, int flags);
  void deactivatedObject(const QString& fullName);
  
private:
  class RegisteredObject : public QTreeWidgetItem
  {
  public:
    RegisteredObject(CalibratorTool::Object* object, QTreeWidgetItem* parentItem, int flags) :
    QTreeWidgetItem(parentItem), object(object), fullName(object->getFullName()), flags(flags), opened(false) {}
    
    CalibratorTool::Object* object;
    const QString fullName;
    int flags;
    bool opened;
  };

  QMenu* contextMenu;
  QTreeWidget* treeWidget;
  QFont italicFont;
  QFont boldFont;
  QSet<QString> expandedItems;
  QHash<const void*, RegisteredObject*> registeredObjectsByObject;
  QHash<int, QHash<QString, RegisteredObject*>*> registeredObjectsByKindAndName;
  
  RegisteredObject* clickedItem;
  
  void deleteRegisteredObject(RegisteredObject* registeredObject);
  
  private slots:
  void itemActivated(const QModelIndex& index);
  void itemCollapsed(const QModelIndex& index);
  void itemExpanded(const QModelIndex& index);
  
  void openOrCloseObject();
  void expandOrCollabseObject();

};
