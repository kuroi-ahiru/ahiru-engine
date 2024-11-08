// GameObject.h
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "Component.h"

class GameObject {
public:
    GameObject(const std::string& name);
    ~GameObject();

    void AddComponent(std::shared_ptr<Component> component);
    void Update();
    void SetActive(bool active);
    bool IsActive() const;
    const std::string& GetName() const;

private:
    std::string name;
    bool active;
    std::vector<std::shared_ptr<Component>> components;
};

#endif // GAMEOBJECT_H
#pragma once
