#pragma once

// The purpose of this structure is simple. Sometimes, you just want to store a unique pointer to -something-
// to wrap the lifetime of resources.
//
// You don't really care what - you just need a base class for it.
//
// The only requirement is for this base class to have a virtual destructor so when it is freed, the
// resources of whatever it owns are also freed.
//
// This is that structure.

struct VirtualObject
{
    virtual ~VirtualObject() = 0;
};
