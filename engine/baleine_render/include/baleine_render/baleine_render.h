#pragma once

#include "baleine_type/atomic.h"
#include "baleine_type/memory.h"
#include "baleine_type/mutex.h"
#include "baleine_type/shared_mutex.h"
#include "baleine_type/thread.h"
#include "baleine_type/vector.h"

namespace baleine {

class Instance;
class Texture;
class Sampler;
class Device;
class Queue;
class CommandPool;
class CommandBuffer;


enum class ResourceState {
    Created,
    Initialized,
    InUse,
    Destroyed
};

class ThreadSafeResource {
private:
    mutable SharedMutex state_mutex;
    Atomic<ResourceState> state { ResourceState::Created };
    ThreadId creating_thread;

public:
    ThreadSafeResource() : creating_thread(std::this_thread::get_id()) {}
    virtual  ~ThreadSafeResource() = default;

    ResourceState get_state() const {
        return state.load(std::memory_order_acquire);
    }

    bool is_usable() const {
        const auto current_state = get_state();
        return current_state == ResourceState::Initialized || current_state == ResourceState::InUse;
    }

protected:
    void set_state(ResourceState new_state) {
        state.store(new_state, std::memory_order_release);
    }

    // Helper to ensure thread-safe state transitions
    bool try_transition_state(ResourceState expected, ResourceState desired) {
        return state.compare_exchange_strong(expected, desired,
                                            std::memory_order_acq_rel);
    }

    // Lock for reading (shared access)
    SharedLock<SharedMutex> ReadLock() const {
        return SharedLock(state_mutex);
    }

    // Lock for writing (exclusive access)
    UniqueLock<SharedMutex> WriteLock() const {
        return UniqueLock(state_mutex);
    }
};

class Instance: public ThreadSafeResource, public EnableSharedFromThis<Instance> {
private:
    mutable Mutex device_creation_mutex;
    Vec<Weak<Device>> created_devices;

public:
    virtual ~Instance() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    Shared<Device> create_device() {
        if (!is_usable()) {
            throw std::runtime_error()
        }
    }

protected:
    virtual Shared<Device> create_device_impl() = 0;
};

class Device: EnableSharedFromThis<Device> {
    Shared<Instance> instance;

public:
    virtual ~Device() = default;
    virtual Shared<Texture> create_image();
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
