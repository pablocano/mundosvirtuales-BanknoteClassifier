//
//  ListViewsDockWidget.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 13-09-16.
//
//
#include <QTreeWidget>
#include <QHeaderView>
#include <QSettings>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QApplication>

#include "ListViewsDockWidget.h"
#include "MainWindow.h"

ListViewsDockWidget::ListViewsDockWidget(QWidget* parent) : QDockWidget(parent)
{
  setAllowedAreas(Qt::TopDockWidgetArea);
  setFocusPolicy(Qt::ClickFocus);
  setObjectName(".SceneGraph");
  setWindowTitle(tr("Scene Graph"));
  treeWidget = new QTreeWidget(this);
  italicFont = treeWidget->font();
  italicFont.setItalic(true);
  boldFont = treeWidget->font();
  boldFont.setBold(true);
  treeWidget->setFrameStyle(QFrame::NoFrame);
  setWidget(treeWidget);
  setFocusProxy(treeWidget);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
  treeWidget->setExpandsOnDoubleClick(false);
#endif
  treeWidget->header()->hide();
  
  connect(treeWidget, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));
  connect(treeWidget, SIGNAL(collapsed(const QModelIndex&)), this, SLOT(itemCollapsed(const QModelIndex&)));
  connect(treeWidget, SIGNAL(expanded(const QModelIndex&)), this, SLOT(itemExpanded(const QModelIndex&)));
  
  // load layout settings
  QSettings& settings = MainWindow::application->getSettings();
  settings.beginGroup(".ListViews");
  expandedItems = QSet<QString>::fromList(settings.value("ExpandedItems").toStringList());
  settings.endGroup();
}

ListViewsDockWidget::~ListViewsDockWidget()
{
  // save layout settings
  QSettings& settings = MainWindow::application->getSettings();
  settings.beginGroup(".ListViews");
  settings.setValue("ExpandedItems", QStringList(expandedItems.values()));
  settings.endGroup();
  
  //
  unregisterAllObjects();
  Q_ASSERT(registeredObjectsByKindAndName.isEmpty());
}

void ListViewsDockWidget::registerObject(CalibratorTool::Object* object, const CalibratorTool::Object* parent, int flags)
{
  QTreeWidgetItem* parentItem = parent ? registeredObjectsByObject.value(parent) : treeWidget->invisibleRootItem();
  RegisteredObject* newItem = new RegisteredObject(object, parentItem, flags);
  int parentFullNameLength = parent ? ((RegisteredObject*)parentItem)->fullName.length() : 0;
  newItem->setText(0, parent ? newItem->fullName.mid(parentFullNameLength + 1) : newItem->fullName);
  const QIcon* icon = object->getIcon();
  if(icon)
    newItem->setIcon(0, *icon);
  if(flags & CalibratorTool::Flag::hidden)
    newItem->setHidden(true);
  if(flags & CalibratorTool::Flag::windowless)
    newItem->setFont(0, italicFont);
  else
    newItem->setDisabled(true);
  parentItem->addChild(newItem);
  if(!parent)
    parentItem->sortChildren(0, Qt::AscendingOrder);
  if(expandedItems.contains(newItem->fullName))
    treeWidget->expandItem(newItem);
  
  registeredObjectsByObject.insert(object, newItem);
  
  int kind = object->getKind();
  QHash<QString, RegisteredObject*>* registeredObjectsByName = registeredObjectsByKindAndName.value(kind);
  if(!registeredObjectsByName)
  {
    registeredObjectsByName = new QHash<QString, RegisteredObject*>();
    registeredObjectsByKindAndName.insert(kind, registeredObjectsByName);
  }
  
  registeredObjectsByName->insert(newItem->fullName, newItem);
  
  if(flags & CalibratorTool::Flag::showParent)
    while(parentItem)
    {
      parentItem->setHidden(false);
      parentItem = parentItem->parent();
    }
}

void ListViewsDockWidget::unregisterAllObjects()
{
  registeredObjectsByObject.clear();
  qDeleteAll(registeredObjectsByKindAndName);
  registeredObjectsByKindAndName.clear();
  treeWidget->clear();
}

bool ListViewsDockWidget::unregisterObject(const CalibratorTool::Object* object)
{
  RegisteredObject* regObject = registeredObjectsByObject.value(object);
  if(!regObject)
    return false;
  deleteRegisteredObject(regObject);
  return true;
}

int ListViewsDockWidget::getObjectChildCount(const CalibratorTool::Object* object)
{
  RegisteredObject* item = registeredObjectsByObject.value(object);
  return item ? item->childCount() : 0;
}

CalibratorTool::Object* ListViewsDockWidget::getObjectChild(const CalibratorTool::Object* object, int index)
{
  RegisteredObject* item = registeredObjectsByObject.value(object);
  return item && index >= 0 && index < item->childCount() ? ((RegisteredObject*)item->child(index))->object : 0;
}

bool ListViewsDockWidget::activateFirstObject()
{
  RegisteredObject* item = (RegisteredObject*)treeWidget->invisibleRootItem()->child(0);
  if(!item)
    return false;
  emit activatedObject(item->fullName, item->object, item->flags);
  return true;
}

bool ListViewsDockWidget::activateObject(const CalibratorTool::Object* object)
{
  RegisteredObject* item = registeredObjectsByObject.value(object);
  if(!item)
    return false;
  emit activatedObject(item->fullName, item->object, item->flags);
  return true;
}

bool ListViewsDockWidget::setOpened(const CalibratorTool::Object* object, bool opened)
{
  RegisteredObject* item = registeredObjectsByObject.value(object);
  if(!item)
    return false;
  item->opened = opened;
  //item->setFont(0, opened ? boldFont : QFont());
  item->setDisabled(!opened);
  if(!opened)
    item->setFont(0, QFont());
  return true;
}

bool ListViewsDockWidget::setActive(const CalibratorTool::Object* object, bool active)
{
  RegisteredObject* item = registeredObjectsByObject.value(object);
  if(!item)
    return false;
  item->setFont(0, active ? boldFont : QFont());
  if(active)
    treeWidget->setCurrentItem(item);
  return true;
}

QAction *ListViewsDockWidget::toggleViewAction() const
{
  QAction* action = QDockWidget::toggleViewAction();
  action->setIcon(QIcon(":/Icons/application_side_tree.png"));
  action->setShortcut(QKeySequence(Qt::Key_F2));
  return action;
}

void ListViewsDockWidget::deleteRegisteredObject(RegisteredObject* registeredObject)
{
  for(int i = registeredObject->childCount() - 1; i >= 0; --i)
    deleteRegisteredObject((RegisteredObject*)registeredObject->child(i));
  registeredObjectsByObject.remove(registeredObject->object);
  int kind = registeredObject->object->getKind();
  QHash<QString, RegisteredObject*>* registeredObjectsByName = registeredObjectsByKindAndName.value(kind);
  if(registeredObjectsByName)
  {
    registeredObjectsByName->remove(registeredObject->fullName);
    if(registeredObjectsByName->count() == 0)
    {
      registeredObjectsByKindAndName.remove(kind);
      delete registeredObjectsByName;
    }
  }
  delete registeredObject;
}

void ListViewsDockWidget::itemActivated(const QModelIndex& index)
{
  RegisteredObject* item = (RegisteredObject*)index.internalPointer();
  if(item->flags & CalibratorTool::Flag::windowless)
  {
    if(item->isExpanded())
      treeWidget->collapseItem(item);
    else
      treeWidget->expandItem(item);
  }
  else
    emit activatedObject(item->fullName, item->object, item->flags);
}

void ListViewsDockWidget::itemCollapsed(const QModelIndex& index)
{
  RegisteredObject* item = (RegisteredObject*)index.internalPointer();
  expandedItems.remove(item->fullName);
}

void ListViewsDockWidget::itemExpanded(const QModelIndex& index)
{
  RegisteredObject* item = (RegisteredObject*)index.internalPointer();
  expandedItems.insert(item->fullName);
}

void ListViewsDockWidget::openOrCloseObject()
{
  if(clickedItem->opened)
    emit deactivatedObject(clickedItem->fullName);
  else
    emit activatedObject(clickedItem->fullName, clickedItem->object, clickedItem->flags);
}

void ListViewsDockWidget::expandOrCollabseObject()
{
  if(clickedItem->isExpanded())
    treeWidget->collapseItem(clickedItem);
  else
    treeWidget->expandItem(clickedItem);
}



