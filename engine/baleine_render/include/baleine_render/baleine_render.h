#pragma once

#include "baleine_type/memory.h"
#include "baleine_type/mutex.h"

namespace baleine {

class Instance;
class Texture;
class Sampler;
class Device;
class Queue;
class CommandPool;
class CommandBuffer;
class Buffer;


enum class ResourceState {
    Created,
    Initialized,
    InUse,
    Destroyed
};

class Instance: public EnableSharedFromThis<Instance> {
public:
    virtual ~Instance() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual Shared<Device> create_device() = 0;
};

class Device: EnableSharedFromThis<Device> {
    Shared<Instance> instance;

public:
    virtual ~Device() = default;
    virtual Shared<Texture> create_image();
    virtual Shared<Buffer> create_buffer();
};

class Buffer: EnableSharedFromThis<Buffer> {

};

class CommandPool: EnableSharedFromThis<CommandPool> {
    Shared<Device> device;
};

class Queue: EnableSharedFromThis<Queue> {

};

class CommandBuffer: EnableSharedFromThis<CommandBuffer> {
    Shared<CommandPool> pool;
};

class Surface: EnableSharedFromThis<Surface> {
    Shared<Device> device;
};

class Texture: EnableSharedFromThis<Texture> {
    Shared<Device> device;
};

class Sampler: EnableSharedFromThis<Sampler> {
    Shared<Device> device;
};

} // namespace baleine
