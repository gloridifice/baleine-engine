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
class Shader;

class Instance: public EnableSharedFromThis<Instance> {
public:
    virtual ~Instance() = 0;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual Shared<Device> create_device() = 0;
};

class Device: EnableSharedFromThis<Device> {
    Shared<Instance> instance;

public:
    virtual ~Device() = 0;
    virtual Shared<Texture> create_texture() = 0;
    virtual Shared<Buffer> create_buffer() = 0;
};

class Buffer: EnableSharedFromThis<Buffer> {
    Shared<Device> device;
};

class CommandPool: EnableSharedFromThis<CommandPool> {
    Shared<Device> device;
};

class Queue: EnableSharedFromThis<Queue> {
    Shared<Device> device;
};

class CommandBuffer: EnableSharedFromThis<CommandBuffer> {
    Shared<CommandPool> pool;

public:
    virtual ~CommandBuffer() = 0;
    virtual void begin_cmd() = 0;
    virtual void end_cmd() = 0;
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

class Shader: EnableSharedFromThis<Shader> {
    
};

}
