// Component.h
#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component {
public:
    enum class Type { Transform, Mesh, Material, Light, Script };

    Component(Type type, GameObject* owner);
    virtual ~Component() = default;

    virtual void Enable();
    virtual void Update();
    virtual void Disable();
    bool IsActive() const;

protected:
    GameObject* owner;
    Type type;
    bool active;
};

#endif // COMPONENT_H
#pragma once
