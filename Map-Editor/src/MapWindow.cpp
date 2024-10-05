#include "MapWindow.h"

#include "ui_MapWindow.h"


MapWindow::MapWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MapWindow), m_floor(-1), m_choice(-1) {
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/MagicTower.ico"));
    ui->mapWidget->setStyleSheet("background-color:rgb(58, 58, 58);");
    ui->listWidget->setStyleSheet("background-color:rgb(58, 58, 58);");

    // 导入所有资源
    QString fname = QDir::cleanPath(TEST_DATA_PATH "/basic/source.json");
    std::fstream fp(fname.toStdString(), std::ios::in);
    fp >> m_sourceData;
    fp.close();

    fname = QDir::cleanPath(TEST_DATA_PATH "/basic/map.json");
    fp.open(fname.toStdString(), std::ios::in);
    fp >> m_mapData;
    fp.close();

    // 导入图片数据
    m_image.load(QDir::cleanPath(TEST_DATA_PATH "/image/objects.png"));

    // 根据资源数据创建图片对象
    for(int i = 0; i < int(m_sourceData.size()); i++) {
        QListWidgetItem* imageItem = createItem(i);
        ui->listWidget->addItem(imageItem);
        m_item.push_back(imageItem);
    }

    // 设置列表显示控件
    ui->listWidget->setViewMode(QListWidget::IconMode);  // 显示模式
    ui->listWidget->setIconSize(QSize(40, 40));          // 设置图片大小
    ui->listWidget->setSpacing(0);                       // 间距
    ui->listWidget->setDragEnabled(false);               // 禁止拖动控件

    // 创建显示第一层地图
    for(int f = 0; f < int(m_mapData.size()); f++) {
        m_floor = f;
        readMap(m_floor);
        break;
    }

    // 设置列表显示控件
    ui->mapWidget->setViewMode(QListWidget::IconMode);  // 显示模式
    ui->mapWidget->setIconSize(QSize(40, 40));          // 设置图片大小
    ui->mapWidget->setSpacing(0);                       // 间距
    ui->mapWidget->setDragEnabled(false);               // 禁止拖动控件

    // 启用长按
    ui->prevButton_2->setAutoRepeat(true);
    ui->nextButton_2->setAutoRepeat(true);
    ui->prevButton->setAutoRepeat(true);
    ui->nextButton->setAutoRepeat(true);
}

MapWindow::~MapWindow() {
    delete ui;
}

QIcon MapWindow::createIcon(int id) {
    QString str = QString::asprintf("id_%d", id);

    // 获得对应 id_i 的位置数据
    int x = m_sourceData[str.toStdString()]["src"]["j"];
    int y = m_sourceData[str.toStdString()]["src"]["i"];

    // 裁剪对应位置的图片进行拉伸，转换为 icon 保存
    QImage subimage = m_image.copy(QRect(x * 33, y * 33, 32, 32)).scaled(40, 40);
    return QPixmap::fromImage(subimage);
}

QListWidgetItem* MapWindow::createItem(int id) {
    // 产生对应控件，并记录 id 数据
    QListWidgetItem* imageItem = new QListWidgetItem;
    imageItem->setIcon(createIcon(id));
    imageItem->setSizeHint(QSize(40, 40));
    imageItem->setData(Qt::UserRole, id);
    imageItem->setBackground(QBrush(QColor(58, 58, 58)));
    return imageItem;
}

void MapWindow::swapItem(int index, int nIndex) {
    // 先交换 listWidget（由于没办法在指定位置插入（只能插入指定行），所以我们交换图片）
    QListWidgetItem* item = m_item[index];
    QListWidgetItem* item2 = m_item[nIndex];
    QIcon icon = item->icon();
    item->setIcon(item2->icon());
    item2->setIcon(icon);

    // 这里同步更新 json 文件，交换两个键保存的值
    QString key1 = QString::asprintf("id_%d", index);
    QString key2 = QString::asprintf("id_%d", nIndex);
    std::swap(m_sourceData[key1.toStdString()], m_sourceData[key2.toStdString()]);

    // 同时更新地图文件
    for(int f = 0; f < int(m_mapData.size()); f++) {
        // 获得层对应的 key
        QString fstr = QString::asprintf("floor_%d", f);
        nlohmann::json& data = m_mapData[fstr.toStdString()]["map"];

        for(int i = 0; i < 13; i++) {
            for(int j = 0; j < 13; j++) {
                if(data[i][j] == index)
                    data[i][j] = nIndex;
                else if(data[i][j] == nIndex)
                    data[i][j] = index;
            }
        }

        // 修改 event 中的 create 消息
        nlohmann::json& event = m_mapData[fstr.toStdString()]["event"];
        for(auto it = event.begin(); it != event.end(); ++it) {
            nlohmann::json& eList = *it;
            for(int i = 0; i < int(eList.size()); i++) {
                // 找到所有 create 命令，修改创建的物体 id
                if(eList[i]["command"] == "create" && !eList[i]["id"].is_null()) {
                    if(eList[i]["id"] == index)
                        eList[i]["id"] = nIndex;
                    else if(eList[i]["id"] == nIndex)
                        eList[i]["id"] = index;
                }
            }
        }
    }

    // 更新选择（注意在更新完 json 以后再更新列表控件，因为此函数会触发 currentItemChanged 槽函数刷新显示）
    ui->listWidget->setCurrentItem(item2);

    // 地图列表控件也要更新
    readMap(m_floor);
}

void MapWindow::readMap(int floor) {
    // 先清空显示
    ui->mapWidget->clear();

    // 刷新标签显示，注意层数+1
    QString str = QString::asprintf("%d/%d", floor + 1, m_mapData.size());
    ui->label->setText(str);

    // 楼层应当非负
    if(floor >= 0) {
        // 获得层对应的 key
        QString fstr = QString::asprintf("floor_%d", floor);
        nlohmann::json data = m_mapData[fstr.toStdString()];

        // 根据此层信息创建显示控件
        for(int i = 0; i < 13; i++) {
            for(int j = 0; j < 13; j++) {
                QListWidgetItem* imageItem = createItem(int(data["map"][i][j]));

                // 附加事件
                QString tag = QString::asprintf("%d_%d", i, j);
                if(!data["event"][tag.toStdString()].is_null())
                    imageItem->setData(Qt::UserRole + 2, tag);
                else
                    imageItem->setData(Qt::UserRole + 2, "none");

                // 这里增加位置信息
                imageItem->setData(Qt::UserRole + 1, QPoint(i, j));
                ui->mapWidget->addItem(imageItem);
            }
        }
    }
}

bool MapWindow::checkValid(std::string json) {
    // 首先清除不需要的字符，中文、tab、换行、空格
    std::string res = "";
    for(int i = 0; i < int(json.size()); i++) {
        // 如果字符高位为 1 则有中文字符
        if(!(json[i] & 0x80) && '	' != json[i] && '\n' != json[i] && ' ' != json[i]) res += json[i];
    }

    // 然后检查 json 格式
    JSON_checker jc = new_JSON_checker(20);
    for(int i = 0; i < int(res.size()); i++) {
        if(!JSON_checker_char(jc, res.at(i))) {
            qDebug() << "JSON_checker_char: syntax error";
            return false;
        }
    }
    if(!JSON_checker_done(jc)) {
        qDebug() << "JSON_checker_char: syntax error";
        return false;
    }
    return true;
}

void MapWindow::on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous) {
    Q_UNUSED(previous)

    // 获得当前项对应的 id
    if(int(m_mapData.size()) > 0) {
        m_choice = current->data(Qt::UserRole).toInt();

        // 显示内容
        QString id = QString::asprintf("id_%d", m_choice);
        QString detail = m_sourceData[id.toStdString()].dump(2).c_str();
        ui->idLabel->setText(id);
        ui->attrEdit->setText(detail);

        // 修改 map 中对应的物体
        QListWidgetItem* item = ui->mapWidget->currentItem();
        if(item != nullptr) {
            item->setIcon(current->icon());
            item->setData(Qt::UserRole, m_choice);

            // 修改 json
            QString fstr = QString::asprintf("floor_%d", m_floor);
            QPoint pos = item->data(Qt::UserRole + 1).toPoint();
            m_mapData[fstr.toStdString()]["map"][pos.x()][pos.y()] = m_choice;
        }
    }
}

void MapWindow::on_mapWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous) {
    Q_UNUSED(previous)

    if(current != nullptr) {
        // 刷新事件编辑框显示
        QString tag = current->data(Qt::UserRole + 2).toString();
        if(tag == "none")
            ui->eventEdit->setText("");
        else if(m_floor >= 0) {
            QString fstr = QString::asprintf("floor_%d", m_floor);
            QString detail = m_mapData[fstr.toStdString()]["event"][tag.toStdString()].dump(2).c_str();
            ui->eventEdit->setText(detail);
        }

        // 将当前选中项设为 list 中选择的图像
        QListWidgetItem* item = ui->listWidget->currentItem();
        if(item != nullptr) {
            current->setIcon(item->icon());
            current->setData(Qt::UserRole, m_choice);

            // 修改 json
            QString fstr = QString::asprintf("floor_%d", m_floor);
            QPoint pos = current->data(Qt::UserRole + 1).toPoint();
            m_mapData[fstr.toStdString()]["map"][pos.x()][pos.y()] = m_choice;
        }
    }
}

void MapWindow::on_saveButton_clicked() {
    // 将结果输出，注意设置缩进格式
    QString fname = QDir::cleanPath(TEST_DATA_PATH "/basic/source.json");
    std::fstream fp(fname.toStdString(), std::ios::out);
    fp << std::setw(4) << m_sourceData;
    fp.close();

    fname = QDir::cleanPath(TEST_DATA_PATH "/basic/map.json");
    fp.open(fname.toStdString(), std::ios::out);
    fp << std::setw(4) << m_mapData;
    fp.close();

    // 提示保存成功
    QMessageBox::information(this, "提示", "保存成功！");
}

void MapWindow::on_prevButton_2_clicked() {
    // 检测没有越界就开始交换
    if(m_choice > 0 && m_choice < m_item.size()) swapItem(m_choice, m_choice - 1);
}

void MapWindow::on_nextButton_2_clicked() {
    // 检测没有越界就开始交换
    if(m_choice >= 0 && m_choice < m_item.size() - 1) swapItem(m_choice, m_choice + 1);
}

void MapWindow::on_prevButton_clicked() {
    // 加载上一层
    if(m_floor > 0) {
        m_floor--;
        readMap(m_floor);
    }
}

void MapWindow::on_nextButton_clicked() {
    // 加载下一层
    if(m_floor < int(m_mapData.size()) - 1) {
        m_floor++;
        readMap(m_floor);
    }
}

void MapWindow::on_newButton_clicked() {
    // 新的一层放到最后面
    int floor = m_mapData.size();
    QString fstr = QString::asprintf("floor_%d", floor);
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++) m_mapData[fstr.toStdString()]["map"][i][j] = 0;
    m_mapData[fstr.toStdString()]["event"] = nlohmann::json::object();

    // 刷新标签显示，注意层数+1
    QString str = QString::asprintf("%d/%d", m_floor + 1, floor + 1);
    ui->label->setText(str);

    // 当没有楼层时，将会自动读取创建的楼层
    m_floor = std::max(m_floor, int(m_mapData.size()) - 1);
    readMap(m_floor);
}

void MapWindow::on_deleteButton_clicked() {
    // 删除最后一层
    int floor = m_mapData.size();
    if(floor > 0 && QMessageBox::question(this, "提示", "是否删除最后一层？") == QMessageBox::Yes) {
        QString fstr = QString::asprintf("floor_%d", floor - 1);
        m_mapData.erase(fstr.toStdString());
        m_floor = std::min(m_floor, int(m_mapData.size()) - 1);
        readMap(m_floor);
    }
}

void MapWindow::on_eventButton_clicked() {
    // 当有选中项，且没有事件时可以创建
    QListWidgetItem* item = ui->mapWidget->currentItem();
    if(item != nullptr && item->data(Qt::UserRole + 2).toString() == "none") {
        // 初始化事件只包括一个空列表
        QPoint pos = item->data(Qt::UserRole + 1).toPoint();
        QString fstr = QString::asprintf("floor_%d", m_floor);
        QString tag = QString::asprintf("%d_%d", pos.x(), pos.y());
        m_mapData[fstr.toStdString()]["event"][tag.toStdString()] = nlohmann::json::array();

        // 创建后，显示在事件编辑框中
        QString detail = nlohmann::json::array().dump(2).c_str();
        ui->eventEdit->setText(detail);

        // 将此事件绑定到 item
        item->setData(Qt::UserRole + 2, tag);
    }
}

void MapWindow::on_delEventButton_clicked() {
    // 当有选中项，且有事件时可以删除
    QListWidgetItem* item = ui->mapWidget->currentItem();
    if(item != nullptr && item->data(Qt::UserRole + 2).toString() != "none") {
        QPoint pos = item->data(Qt::UserRole + 1).toPoint();
        QString fstr = QString::asprintf("floor_%d", m_floor);
        QString tag = QString::asprintf("%d_%d", pos.x(), pos.y());

        // 消除事件绑定
        item->setData(Qt::UserRole + 2, "none");
        m_mapData[fstr.toStdString()]["event"].erase(tag.toStdString());
        ui->eventEdit->setText("");
    }
}

void MapWindow::on_attrEdit_textChanged() {
    QListWidgetItem* item = ui->listWidget->currentItem();
    if(item != nullptr) {
        std::string id = QString::asprintf("id_%d", m_choice).toStdString();
        std::string plainText = ui->attrEdit->toPlainText().toStdString();

        // 先要检查有效性
        if(checkValid(plainText)) {
            // 由于我们清除了中文名称，这里需要重新添加
            nlohmann::json name = m_sourceData[id]["name"];
            m_sourceData[id] = nlohmann::json::parse(plainText);
            m_sourceData[id]["name"] = name;

            ui->tipLabel->setText("有效 JSON");

            // 修改可能的图标
            for(int i = 0; i < m_item.size(); i++) m_item[i]->setIcon(createIcon(i));

            // 重新读取楼层
            readMap(m_floor);
        } else
            ui->tipLabel->setText("无效 JSON");
    }
}

void MapWindow::on_eventEdit_textChanged() {
    QListWidgetItem* item = ui->mapWidget->currentItem();
    if(item != nullptr) {
        std::string plainText = ui->eventEdit->toPlainText().toStdString();

        // 先要检查有效性
        if(m_floor >= 0 && checkValid(plainText)) {
            QPoint pos = item->data(Qt::UserRole + 1).toPoint();
            QString fstr = QString::asprintf("floor_%d", m_floor);
            QString tag = QString::asprintf("%d_%d", pos.x(), pos.y());
            m_mapData[fstr.toStdString()]["event"][tag.toStdString()] = nlohmann::json::parse(plainText);
            ui->tipLabel->setText("有效 JSON");
        } else
            ui->tipLabel->setText("无效 JSON");
    }
}
