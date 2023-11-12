#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <optional>
#include <vector>
#include <set>


class TriApp {
public: 
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window_;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    VkInstance instance_;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() &&
                presentFamily.has_value();
        }
    };

    VkDevice device_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;

    VkSurfaceKHR surface_;

    void createLogicalDevice() {
        VkPhysicalDeviceFeatures features{};
        vkGetPhysicalDeviceFeatures(physicalDevice_, &features);

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> queueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (const auto& queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &features;

        if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
    }

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            i++;
        }
        return indices;
    }

    void initVulkan(){
        createInstance();
        createSurface();
        setupDevices();
        createLogicalDevice();
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void createInstance() {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Hello Triangle";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName =  "No Engine";
        applicationInfo.engineVersion =  VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;
    
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void setupDevices() {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> availableDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance_, &deviceCount, availableDevices.data());

        for (const auto& availableDevice : availableDevices) {
            if (isDeviceSuitable(availableDevice)) {
                physicalDevice_ = availableDevice;
                break;
            }
        }

        if (physicalDevice_ == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(const VkPhysicalDevice& device) {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(device, &properties);
        vkGetPhysicalDeviceFeatures(device, &features);

        QueueFamilyIndices indices = findQueueFamilies(device);
        return features.geometryShader &&
            indices.isComplete();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyDevice(device_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkDestroyInstance(instance_, nullptr);

        glfwDestroyWindow(window_);
        glfwTerminate();
    }    
};

int main() {
    TriApp app;
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
