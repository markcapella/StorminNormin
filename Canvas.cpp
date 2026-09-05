
#include "Global.h"

/**
 * StorminNormin canvas constructor.
 */
Canvas::Canvas(Window window) {
    mWindow = window;
}

/**
 * StorminNormin canvas destructor.
 */
Canvas::~Canvas() {
    uninitCanvas();
}

/**
 * Init method for Vulkan graphics tools.
 */
bool
Canvas::initCanvas() {
    if (mInitialized) {
        return true;
    }

    mCanvasWidth = mSettingsHelper->getCanvasWidth();
    mCanvasHeight = mSettingsHelper->getCanvasHeight();

    const XRenderColor FLAKE_COLOR_ONE = mSettingsHelper->
        getColorSetting(SettingsHelper::FROSTEDFLAKES_COLOR_ONE);
    mFlakeColors[0] = FLAKE_COLOR_ONE;

    const XRenderColor FLAKE_COLOR_TWO = mSettingsHelper->
        getColorSetting(SettingsHelper::FROSTEDFLAKES_COLOR_TWO);
    mFlakeColors[1] = FLAKE_COLOR_TWO;

    const XRenderColor FLAKE_COLOR_THREE = mSettingsHelper->
        getColorSetting(SettingsHelper::FROSTEDFLAKES_COLOR_THREE);
    mFlakeColors[2] = FLAKE_COLOR_THREE;

    const XRenderColor FLAKE_COLOR_FOUR = mSettingsHelper->
        getColorSetting(SettingsHelper::FROSTEDFLAKES_COLOR_FOUR);
    mFlakeColors[3] = FLAKE_COLOR_FOUR;

    mStormItemCount = (float) mSettingsHelper->getIntSetting(
        SettingsHelper::FROSTEDFLAKES_SATURATION);

    mFlakeRadiusX = (mSettingsHelper->getIntSetting(SettingsHelper::
        FROSTEDFLAKES_SIZE) + 1) / static_cast<float>(mCanvasWidth);
    mFlakeRadiusY = (mSettingsHelper->getIntSetting(SettingsHelper::
        FROSTEDFLAKES_SIZE) + 1) / static_cast<float>(mCanvasHeight);

    mWindowCornerWidth = 2.0f * (Button::BUTTON_WIDTH /
        static_cast<float>(mCanvasWidth));
    mWindowCornerHeight = 2.0f * (Button::BUTTON_WIDTH /
        static_cast<float>(mCanvasHeight));

    try {
        // Vulkan Instance.
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        const char* instanceExtensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME
        };

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 2;
        createInfo.ppEnabledExtensionNames = instanceExtensions;

        if (vkCreateInstance(&createInfo, nullptr, &mInstance) !=
            VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create instance! Ensure "
                "your drivers support Vulkan 1.3" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Window Surface.
        VkXlibSurfaceCreateInfoKHR surfaceCreateInfo{};
        surfaceCreateInfo.sType =
            VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.dpy = mDisplay;
        surfaceCreateInfo.window = mWindow;

        if (vkCreateXlibSurfaceKHR(mInstance, &surfaceCreateInfo,
            nullptr, &mSurface) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create X11 "
                "Vulkan surface!" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Physical Device & Queue Family.
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            cout << XCOLOR_RED << "Failed to find GPUs with "
                "Vulkan support!" << endl << endl;
            uninitCanvas();
            return false;
        }
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount,
            devices.data());
        mPhysicalDevice = devices[0];

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice,
            &queueFamilyCount, nullptr);
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice,
            &queueFamilyCount, queueFamilies.data());

        mGraphicsQueueFamilyIndex = -1;
        for (int i = 0; i < static_cast<int>(queueFamilyCount); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                mGraphicsQueueFamilyIndex = i;
                break;
            }
        }

        if (mGraphicsQueueFamilyIndex == -1) {
            cout << XCOLOR_RED << "Failed to find a graphics "
                "queue family!" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Logical Device.
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(
            mGraphicsQueueFamilyIndex);
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceDynamicRenderingFeatures
            dynamicRenderingFeatures{};
        dynamicRenderingFeatures.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

        const vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &dynamicRenderingFeatures;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(
            deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames =
            deviceExtensions.data();

        VkResult result = vkCreateDevice(mPhysicalDevice,
            &deviceCreateInfo, nullptr, &mDevice);
        if (result != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create logical "
                "device! Error code: " << result << "." << endl << endl;
            uninitCanvas();
            return false;
        }

        vkGetDeviceQueue(mDevice, static_cast<uint32_t>(
            mGraphicsQueueFamilyIndex), 0, &mGraphicsQueue);

        // Swapchain.
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType =
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = mSurface;
        swapchainCreateInfo.minImageCount = 2;
        swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        swapchainCreateInfo.imageColorSpace =
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.imageExtent = {
            mCanvasWidth, mCanvasHeight };
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.preTransform =
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha =
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo,
            nullptr, &mSwapchain) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create "
                "swapchain!\n" << endl << endl;
            uninitCanvas();
            return false;
        }

        uint32_t imageCount = 0;
        vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount,
            nullptr);
        mSwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount,
            mSwapchainImages.data());

        // Swapchain Image Views.
        mSwapchainImageViews.resize(imageCount, VK_NULL_HANDLE);
        for (size_t i = 0; i < mSwapchainImages.size(); i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = mSwapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
            viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };
            viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT,
                0, 1, 0, 1 };

            if (vkCreateImageView(mDevice, &viewInfo, nullptr,
                &mSwapchainImageViews[i]) != VK_SUCCESS) {
                cout << XCOLOR_RED << "Failed to create "
                    "swapchain image view " << i << "!" << endl << endl;
                uninitCanvas();
                return false;
            }
        }

        // Pipeline Layout & Shaders.
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstants);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo,
            nullptr, &mPipelineLayout) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create graphics "
                "pipeline layout!" << endl << endl;
            uninitCanvas();
            return false;
        }

        mVertShaderModule = getShaderModule(getShaderFile(
            "flakeShader.vert.spv"));
        mFragShaderModule = getShaderModule(getShaderFile(
            "flakeShader.frag.spv"));

        // Graphics Pipeline.
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = mVertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = mFragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderStageInfo, fragShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

        VkViewport viewport{ 0.0f, 0.0f, static_cast<float>(mCanvasWidth),
            static_cast<float>(mCanvasHeight), 0.0f, 1.0f };
        VkRect2D scissor{ {0, 0}, {mCanvasWidth, mCanvasHeight} };

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor =
            VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor =
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
        pipelineRenderingCreateInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        pipelineRenderingCreateInfo.colorAttachmentCount = 1;
        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        pipelineRenderingCreateInfo.pColorAttachmentFormats =
            &colorFormat;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType =
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &pipelineRenderingCreateInfo;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = mPipelineLayout;

        if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1,
            &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create "
                "graphics pipeline!" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Command Pool & Command Buffer.
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = static_cast<uint32_t>(
            mGraphicsQueueFamilyIndex);

        if (vkCreateCommandPool(mDevice, &poolInfo, nullptr,
            &mCommandPool) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create "
                "command pool!" << endl << endl;
            uninitCanvas();
            return false;
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType =
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(mDevice, &allocInfo,
            &mCommandBuffer) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to allocate "
                "command buffer!" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Synchronization Primitives.
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                &mImageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr,
                &mRenderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(mDevice, &fenceInfo, nullptr,
                &mInFlightFence) != VK_SUCCESS) {
            cout << XCOLOR_RED << "Failed to create "
                "synchronization objects!" << endl << endl;
            uninitCanvas();
            return false;
        }

        // Custom domain objects.
        initStormItems();

    } catch (const exception& e) {
        cout << XCOLOR_RED << "error: " << e.what() << 
            "." << endl << endl;
        uninitCanvas();
        return false;
    }

    mInitialized = true;
    return true;
}

/**
 * Uninit method for Vulkan graphics tools.
 */
void
Canvas::uninitCanvas() {
    if (mDevice != VK_NULL_HANDLE) {
        // Wait for GPU work to complete.
        if (mInFlightFence != VK_NULL_HANDLE) {
            vkWaitForFences(mDevice, 1, &mInFlightFence, VK_TRUE,
                UINT64_MAX);
        }
        vkDeviceWaitIdle(mDevice);

        // Synchronization primitives.
        if (mRenderFinishedSemaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(mDevice, mRenderFinishedSemaphore,
                nullptr);
            mRenderFinishedSemaphore = VK_NULL_HANDLE;
        }
        if (mImageAvailableSemaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(mDevice, mImageAvailableSemaphore,
                nullptr);
            mImageAvailableSemaphore = VK_NULL_HANDLE;
        }
        if (mInFlightFence != VK_NULL_HANDLE) {
            vkDestroyFence(mDevice, mInFlightFence, nullptr);
            mInFlightFence = VK_NULL_HANDLE;
        }

        // Command Pool (automatically frees mCommandBuffer).
        if (mCommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
            mCommandPool = VK_NULL_HANDLE;
            mCommandBuffer = VK_NULL_HANDLE;
        }

        // Pipeline & Layout & Shaders.
        if (mGraphicsPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
            mGraphicsPipeline = VK_NULL_HANDLE;
        }
        if (mPipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
            mPipelineLayout = VK_NULL_HANDLE;
        }
        if (mVertShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(mDevice, mVertShaderModule, nullptr);
            mVertShaderModule = VK_NULL_HANDLE;
        }
        if (mFragShaderModule != VK_NULL_HANDLE) {
            vkDestroyShaderModule(mDevice, mFragShaderModule, nullptr);
            mFragShaderModule = VK_NULL_HANDLE;
        }

        // Swapchain Image Views & Swapchain.
        for (VkImageView imageView : mSwapchainImageViews) {
            if (imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(mDevice, imageView, nullptr);
            }
        }
        mSwapchainImageViews.clear();
        mSwapchainImages.clear();

        if (mSwapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
            mSwapchain = VK_NULL_HANDLE;
        }

        // Free custom sub-system resources.
        uninitStormItems();

        // Logical Device.
        vkDestroyDevice(mDevice, nullptr);
        mDevice = VK_NULL_HANDLE;
        mGraphicsQueue = VK_NULL_HANDLE;
    }

    // Surface & Instance.
    if (mInstance != VK_NULL_HANDLE) {
        if (mSurface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
            mSurface = VK_NULL_HANDLE;
        }
        vkDestroyInstance(mInstance, nullptr);
        mInstance = VK_NULL_HANDLE;
    }

    mInitialized = false;
}

/**
* Draw the canvas and all storm items.
 */
void
Canvas::drawCanvas() {
    // Init on first draw. Careful, it can fail.
    if (!mInitialized) {
        initCanvas();
        if (!mInitialized) {
            return;
        }
    }

    // Assuming ~60 FPS (dt ≈ 0.0166s).
    const double dt = 1.0 / 60.0;
    const float dt_f = static_cast<float>(dt);
    static double totalAppTime = 0.0;
    totalAppTime += dt;

    // Emulate STARTNEW_WIND_THREAD_TIME (every 1.0s).
    mStartNewWindTimer += dt;
    if (mStartNewWindTimer >= 1.0) {
        updateWindGustStrength();
        mStartNewWindTimer -= 1.0;
    }

    // Emulate UPDATE_WIND_THREAD_TIME (every 0.1s).
    mUpdateWindTimer += dt;
    if (mUpdateWindTimer >= 0.1) {
        updateWindGustDirectionAndTime(totalAppTime);
        mUpdateWindTimer -= 0.1;
    }

    // Target NDC Wind, smooth global wind transition.
    float targetNdcWind = mWindStrength * 0.000015f;
    mCurrentWindX += (targetNdcWind - mCurrentWindX) * 0.015f;

    // Physics Constants.
    const float GRAVITY = 1.2f;
    const float DRAG_MULTIPLIER = 4.0f;

    // Particle Update Loop.
    for (int i = 0; i < mStormItemCount; i++) {
        // Per-Flake Wind Variation. Mutate the global wind
        // slightly for this specific flake using its unique
        // mFlakeVariation so no two flakes experience the exact
        // same wind strength. Varies between 0.7x and 1.3x.
        float flakeWindMod = 0.7f + (mFlakeVariation[i] * 0.6f);
        float airVelX = (mCurrentWindX * flakeWindMod) * 60.0f;

        float currentVelX = mSpeedX[i] * 60.0f;
        float currentVelY = mSpeedY[i] * 60.0f;

        // Relative velocity.
        float relVelX = airVelX - currentVelX;
        float relVelY = 0.0f - currentVelY;

        // Aerodynamic drag scaled by each wind sens & mass.
        float dragCoeff = mWindSensitivity[i] * DRAG_MULTIPLIER;
        float dragForceX = dragCoeff * relVelX;
        float dragForceY = dragCoeff * relVelY;

        // Gravity.
        float gravForceY = mFlakeMass[i] * GRAVITY;

        // Acceleration = Force / Mass.
        float accelX = dragForceX / mFlakeMass[i];
        float accelY = (gravForceY + dragForceY) / mFlakeMass[i];

        // Integrate velocity.
        currentVelX += accelX * dt_f;
        currentVelY += accelY * dt_f;

        // Damping when air is calm.
        if (abs(airVelX) < 0.1f) {
            currentVelX *= 0.92f;
        }

        // Independent Micro-Turbulence. Give each flake its own chaotic
        // vertical/horizontal jitter tied to its rotation/variation
        // rather than a shared global random call.
        float noiseX = (static_cast<float>(
            rand()) / RAND_MAX - 0.5f) * 0.4f;
        float noiseY = (static_cast<float>(
            rand()) / RAND_MAX - 0.5f) * 0.4f;

        currentVelX += noiseX * (2.0f / mFlakeMass[i]) * dt_f;
        currentVelY += (mInitialSpeedY[i] * 60.0f + noiseY) *
            0.2f * dt_f;

        // Convert back to per-frame step increments.
        mSpeedX[i] = currentVelX * dt_f;
        mSpeedY[i] = currentVelY * dt_f;

        // Apply Positions & Rotation.
        mFlakeX[i] += mSpeedX[i];
        mFlakeY[i] += mSpeedY[i];

        // Rotation driven by horizontal acceleration
        // & speed differential.
        mFlakeRotation[i] += mSpinSpeed[i] + (mSpeedX[i] * 3.0f *
            mFlakeVariation[i]);

        // Respawn & Edge Wrapping.
        if (mFlakeY[i] - mFlakeRadiusY >= 1.0f) {
            updateStormItem(i, false);
        }

        // Horizontal Screen Wrap.
        if (mFlakeX[i] + mFlakeRadiusX <= -1.0f) {
            mFlakeX[i] = 1.0f + mFlakeRadiusX;
        } else if (mFlakeX[i] - mFlakeRadiusX >= 1.0f) {
            mFlakeX[i] = -1.0f - mFlakeRadiusX;
        }
    }

    // Render frame.
    vkWaitForFences(mDevice, 1, &mInFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(mDevice, 1, &mInFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX,
        mImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(mCommandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(mCommandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.image = mSwapchainImages[imageIndex];
    barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(mCommandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    // Define Rednering attachment info & color attachment,
    // set background.
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = mSwapchainImageViews[imageIndex];
    colorAttachment.imageLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    const XRenderColor BACKGROUND_COLOR = mSettingsHelper->
        getColorSetting(SettingsHelper::BACKGROUND_COLOR);
    const int BACKGROUND_OPACITY = mSettingsHelper->
        getIntSetting(SettingsHelper::BACKGROUND_OPACITY);
    const XRenderColor BLENDED_BACKGROUND = newRenderColor(
        BACKGROUND_COLOR.red, BACKGROUND_COLOR.green,
        BACKGROUND_COLOR.blue, BACKGROUND_OPACITY);
    colorAttachment.clearValue.color = {
        static_cast<float>(BLENDED_BACKGROUND.red) / 65535.0f,
        static_cast<float>(BLENDED_BACKGROUND.green) / 65535.0f,
        static_cast<float>(BLENDED_BACKGROUND.blue) / 65535.0f,
        static_cast<float>(BLENDED_BACKGROUND.alpha) / 65535.0f
    };

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, {mCanvasWidth, mCanvasHeight}};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(mCommandBuffer, &renderingInfo);
    vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        mGraphicsPipeline);

    for (int i = 0; i < mStormItemCount; i++) {
        PushConstants constants{
            mFlakeX[i], mFlakeY[i],
            mFlakeRadiusX, mFlakeRadiusY,
            static_cast<float>(mFlakeColor[i].red) / 65535.0f,
            static_cast<float>(mFlakeColor[i].green) / 65535.0f,
            static_cast<float>(mFlakeColor[i].blue) / 65535.0f,
            static_cast<float>(mFlakeColor[i].alpha) / 65535.0f,
            mFlakeRotation[i],
            mFlakeVariation[i]
        };
        vkCmdPushConstants(mCommandBuffer, mPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants),
            &constants);
        vkCmdDraw(mCommandBuffer, 4, 1, 0, 0);
    }

    vkCmdEndRendering(mCommandBuffer);

    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;
    vkCmdPipelineBarrier(mCommandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0,
        nullptr, 0, nullptr, 1, &barrier);

    vkEndCommandBuffer(mCommandBuffer);

    VkSemaphore waitSemaphores[] = {mImageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphore};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFence);

    VkSwapchainKHR swapchains[] = {mSwapchain};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(mGraphicsQueue, &presentInfo);
}

/**
 * Init StorminNormin canvas & its storm items array.
 */
void
Canvas::initStormItems() {
    srand(41);

    // Setup Wind defaults.
    mWhirlStrength = 0.01 * INITIAL_WHIRL_STRENGTH *
        mWhirlStrengthSlider;
    mWhirlStartTime = (mWhirlTimeSlider < 3.0) ?
        3.0 : mWhirlTimeSlider;
    mWhirlGustDuration = mWhirlStartTime;

    mPrevWindAvailable = false;
    mWindEnabled = 0;
    mWindStrength = 100.0f;
    mCurrentWindX = 0.0f;

    mStartNewWindTimer = 0.0;
    mUpdateWindTimer = 0.0;

    mFlakeX.resize(mStormItemCount);
    mFlakeY.resize(mStormItemCount);
    mSpeedX.resize(mStormItemCount);
    mSpeedY.resize(mStormItemCount);
    mInitialSpeedY.resize(mStormItemCount);

    mFlakeColor.resize(mStormItemCount);
    mFlakeRotation.resize(mStormItemCount);
    mSpinSpeed.resize(mStormItemCount);
    mFlakeVariation.resize(mStormItemCount);

    mFlakeMass.resize(mStormItemCount);
    mWindSensitivity.resize(mStormItemCount);

    for (int i = 0; i < mStormItemCount; i++) {
        updateStormItem(i, true);
    }
}

/**
 * Uninit StorminNormin canvas.
 */
void
Canvas::uninitStormItems() {
    mFlakeX.clear();
    mFlakeY.clear();
    mSpeedX.clear();
    mSpeedY.clear();
    mFlakeColor.clear();
    mFlakeRotation.clear();
    mSpinSpeed.clear();
    mFlakeVariation.clear();
    mFlakeMass.clear();
    mWindSensitivity.clear();
}

/**
 * Update each storm items position, etc during the storm.
 */
void
Canvas::updateStormItem(int particle, bool randomY) {
    mFlakeX[particle] = -1.0f + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / 2.0f));
    if (randomY) {
        mFlakeY[particle] = -1.0f + static_cast<float>(rand()) /
            (static_cast<float>(RAND_MAX / 2.0f));
    } else {
        mFlakeY[particle] = -1.0f - mFlakeRadiusY;
    }

    mFlakeColor[particle] = mFlakeColors[rand() % 4];
    mFlakeVariation[particle] = static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / 10.0f));
    mSpinSpeed[particle] = -0.01f + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / 0.02f));
    mFlakeRotation[particle] = static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / 6.28318f));

    const float minMass = 0.4f;
    const float maxMass = 2.8f;
    mFlakeMass[particle] = minMass + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / (maxMass - minMass)));

    const float minSensitivity =  0.3f;
    const float maxSensitivity = 2.0f;
    mWindSensitivity[particle] = minSensitivity + static_cast<float>(
        rand()) / (static_cast<float>(RAND_MAX /
            (maxSensitivity - minSensitivity)));

    const float minBaseSpeed = 0.008f;
    const float maxBaseSpeed = 0.012f;
    const float baseSpeed = minBaseSpeed + static_cast<float>(rand()) /
        (static_cast<float>(RAND_MAX / (maxBaseSpeed - minBaseSpeed)));

    mSpeedX[particle] = 0.0f;
    mSpeedY[particle] = baseSpeed;
    mInitialSpeedY[particle] = baseSpeed;
}

/**
 * Routinely starts a new wind gust strength.
 */
void
Canvas::updateWindGustStrength() {

    switch (mWindEnabled) {
        case 0:
        default: {
            // Ambient random wander with a strong pull towards zero.
            const float RESULT = (static_cast<float>
                (rand()) / RAND_MAX) * mWhirlStrength;
            mWindStrength += RESULT - (mWhirlStrength / 2.0f);

            // **Stronger dampening**: Actively pull ambient wind
            // toward 0 so it dies down completely.
            mWindStrength *= 0.75f;

            // If it gets very close to zero, kill it completely to
            // allow dead calms.
            if (mWindStrength > -5.0f && mWindStrength < 5.0f) {
                mWindStrength = 0.0f;
            }

            // Hard limits.
            if (mWindStrength > MAX_WIND_STRENGTH) {
                mWindStrength = MAX_WIND_STRENGTH;
            }
            if (mWindStrength < -MAX_WIND_STRENGTH) {
                mWindStrength = -MAX_WIND_STRENGTH;
            }
            break;
        }

        case 1: {
            // Sustained directional gust.
            mWindStrength = mWindDirection * 0.6f * mWhirlStrength;
            break;
        }
    }
}

/**
 * Routinely updates wind gust state (on / off), &
 * direction & duration.
 */
void
Canvas::updateWindGustDirectionAndTime(
    double currentTimeSeconds) {

    // Sanity check duration.
    if (!mPrevWindAvailable) {
        mPrevWindTime = currentTimeSeconds;
        mPrevWindAvailable = true;
    }
    const double ELAPSED = currentTimeSeconds - mPrevWindTime;
    mPrevWindTime = currentTimeSeconds;
    const double THRESHOLD = 2.0 * mWhirlGustDuration *
        (static_cast<double>(rand()) / RAND_MAX);
    if (ELAPSED < THRESHOLD) {
        return;
    }

    // Gust lasts ~5 seconds on average.
    if ((static_cast<double>(rand()) / RAND_MAX) > 0.65) {
        mWindEnabled = 1;
        mWhirlGustDuration = 5.0;
        mWindDirection = static_cast<double>(rand()) /
            RAND_MAX > 0.4 ? 1 : -1;

    // Return to long calm interval (~50s).
    } else {
        mWindEnabled = 0;
        mWhirlGustDuration = mWhirlStartTime;
    }
}

/**
 * Provides a binary shader from app resources folder.
 */
vector<char>
Canvas::getShaderFile(const string& filename) {
    const string FILEPATH = string(SHADER_DIR) + filename;
    ifstream file(FILEPATH, ios::ate | ios::binary);
    if (!file.is_open()) {
        throw runtime_error("failed to open shader file: " + FILEPATH);
    }

    const size_t FILE_SIZE = static_cast<size_t>(file.tellg());
    vector<char> buffer(FILE_SIZE);

    file.seekg(0);
    file.read(buffer.data(), FILE_SIZE);
    file.close();

    return buffer;
}

/**
 * Instantiates a ShaderModule from a binary shader.
 */
VkShaderModule
Canvas::getShaderModule(const vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(mDevice, &createInfo, nullptr,
        &shaderModule) != VK_SUCCESS) {
        throw runtime_error("failed to create shader module!");
    }

    return shaderModule;
}
