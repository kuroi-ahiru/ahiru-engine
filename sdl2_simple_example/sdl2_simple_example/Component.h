#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component {
public:
    enum class Type { Mesh, Transform, Texture };

    Component(Type type, GameObject* owner) : type(type), owner(owner) {}
    virtual ~Component() = default;

    virtual void Update() = 0;
    virtual void Render() = 0;

    Type GetType() const { return type; } //getter para obtener tipo de componente para el inspector

protected:
    Type type;
    GameObject* owner;
};

#endif // COMPONENT_H
