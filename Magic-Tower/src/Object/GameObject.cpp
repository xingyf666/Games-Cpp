#include "GameObject.h"

// 导入所有图片资源
QImage GameObject::s_objImg;
QListWidget* GameObject::s_toolList = nullptr;
nlohmann::json GameObject::s_sourceData;

void GameObject::initObject() {
    s_objImg.load(QDir::cleanPath(TEST_DATA_PATH "/image/objects.png"));
    QString fname = QDir::cleanPath(TEST_DATA_PATH "/basic/source.json");
    std::fstream fp(fname.toStdString(), std::ios::in);
    fp >> s_sourceData;
    fp.close();
}

GameObject::GameObject(QPoint pos, QRect rect): m_event(nullptr), m_rect(rect) {
    m_attributes["i"] = pos.x();
    m_attributes["j"] = pos.y();
    m_attributes["status"] = GameObject::active;
}

GameObject::~GameObject() {
}

int GameObject::operator[](std::string name) const {
    return m_attributes[name];
}

int& GameObject::operator[](std::string name) {
    return m_attributes[name];
}

void GameObject::push(std::string name) {
    m_attrList.push_back(name);
}

void GameObject::bind(GameEvent* event) {
    m_event = event;
}
