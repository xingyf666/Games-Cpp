#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QImage>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QString>
#include <fstream>

#include "JSON_checker.h"
#include "json.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
    class MapWindow;
}
QT_END_NAMESPACE

class MapWindow : public QMainWindow {
    Q_OBJECT

  public:
    MapWindow(QWidget* parent = nullptr);
    ~MapWindow();

    QIcon createIcon(int id);              // 创建 icon
    QListWidgetItem* createItem(int id);   // 创建项
    void swapItem(int index, int nIndex);  // 交换两项
    void readMap(int floor);               // 读取对应层的地图
    bool checkValid(std::string json);     // 检查 json 文件的有效性

  private slots:
    void on_saveButton_clicked();
    void on_prevButton_2_clicked();
    void on_nextButton_2_clicked();
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_newButton_clicked();
    void on_deleteButton_clicked();
    void on_eventButton_clicked();
    void on_delEventButton_clicked();

    void on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void on_mapWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

    void on_attrEdit_textChanged();
    void on_eventEdit_textChanged();

  private:
    Ui::MapWindow* ui;

    int m_floor;                     // 当前楼层，从 0 开始
    int m_choice;                    // 记录选中的 id
    QImage m_image;                  // 保存资源图像
    nlohmann::json m_sourceData;     // 所有资源数据
    nlohmann::json m_mapData;        // 保存地图信息
    QList<QListWidgetItem*> m_item;  // 所有资源图片
};
#endif  // MAPWINDOW_H
