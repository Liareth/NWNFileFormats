#pragma once

#include "Utility/VirtualObject.hpp"

// An RAII wrapper is a simple wrapper that takes moved-in object and stores it.
// That's all it does.
// The hook is that it inherits from a VirtualObject - so you can store anything in this
// thing, and keep a unique_ptr to the VirtualObject. This is powerful because you can
// have a class which can store different types of objects based on internal implementation
// details and then use RAII to free the one that is used automatically.

template <typename T>
struct RAIIWrapper : public VirtualObject
{
    T m_Data;
    RAIIWrapper(T&& data) : m_Data(std::forward<T>(data)) { }
    virtual ~RAIIWrapper() { }
};
