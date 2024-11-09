#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component {
public:
    enum class Type { Mesh };

    Component(Type type, GameObject* owner) : type(type), owner(owner) {}
    virtual ~Component() = default;

    virtual void Update() = 0;
    virtual void Render() = 0;

protected:
    Type type;
    GameObject* owner;
};

#endif // COMPONENT_H
