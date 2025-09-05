
#include "baleine_vulkan/Instance.h"

balkan::Instance::Instance(const char* app_name) {
    vkb::InstanceBuilder instance_builder;

    vkb_instance = instance_builder.set_app_name(app_name)
                       .require_api_version(1, 3, 0)
                       .request_validation_layers(true)
                       .build()
                       .value();

    instance = vkb_instance.instance;
}

balkan::Instance::~Instance() {
    vkDestroyInstance(instance, nullptr);
};