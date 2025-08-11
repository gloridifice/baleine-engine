#pragma once

#include "baleine_type/primitive.h"
#include "baleine_type/vector.h"
#include "vulkan/vulkan.hpp"

namespace baleine {
class RenderContext {
  public:
    vk::Instance instance;
    Vec<vk::PhysicalDevice> physical_devices;
    vk::PhysicalDevice physical_device;
    vk::PhysicalDeviceProperties device_properties;
    vk::PhysicalDeviceFeatures device_features;
    vk::PhysicalDeviceMemoryProperties device_memory_properties;

    u32 version;
    u32 driverVersion;

    void create_context() {
        {
            vk::ApplicationInfo app_info;
            app_info.pApplicationName = "Baleine engine";
            app_info.applicationVersion = 1;
            app_info.apiVersion = VK_API_VERSION_1_3;
            app_info.pEngineName = "Baleine";
            app_info.engineVersion = 1;

            vk::InstanceCreateInfo instance_create_info;
            instance_create_info.pApplicationInfo = &app_info,

            instance = vk::createInstance(instance_create_info);
        }

        physical_devices = instance.enumeratePhysicalDevices();
        physical_device = physical_devices[0];
        device_properties = physical_device.getProperties();
        version = device_properties.apiVersion;
        driverVersion = device_properties.driverVersion;
        device_features = physical_device.getFeatures();
        device_memory_properties = physical_device.getMemoryProperties();
    }
};
} // namespace baleine
