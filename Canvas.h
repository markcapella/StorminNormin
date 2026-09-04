#pragma once

// Standard C libraries.
#include <array>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
using namespace std;

// x11 libraries.
#include <X11/Xlib.h>

// Vulkan libraries.
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.h>

// Push constant layout.
struct alignas(16) PushConstants {
    float positionX, positionY; // offset  0 -> vec2
    float scaleX, scaleY; // offset  8 -> vec2
    float colorR, colorG, colorB, colorA; // offset 16 -> vec4
    float rotation; // offset 32 -> float
    float variation; // offset 36 -> float
};

/**
 *
 */
class Canvas {

    public:

        /**
         * StorminNormin canvas constructor.
         */
        Canvas(Window window);

        /**
         * StorminNormin canvas destructor.
         */
        ~Canvas();

        /**
         * Init method for Vulkan graphics tools.
         */
        bool initVulcanCanvas();

        /**
         * Uninit method for Vulkan graphics tools.
         */
        void uninitVulcanCanvas();

        /**
         * Draw the canvas and all storm items.
         */
        void drawCanvas();

    private:

        /**
         * Init StorminNormin canvas & its storm items array.
         */
        void initStormCanvas();

        /**
         * Uninit StorminNormin canvas.
         */
        void uninitStormCanvas();

        /**
         * Update each storm items position, etc during the storm.
         */
        void updateStormCanvasItem(int index, bool randomY = false);

        /**
          * Routinely starts a new wind gust strength.
         */
        void updateWindGustStrength();

        /**
         * Routinely updates wind gust state (on / off), &
         * direction & duration.
         */
        void updateWindGustDirectionAndTime(double currentTimeSeconds);

        /**
         * Provides a binary shader from app resources folder.
         */
        vector<char> getShaderFile(const string& filename);

        /**
         * Instantiates a ShaderModule from a binary shader.
         */
        VkShaderModule getShaderModule(const vector<char>& code);

        /**
         * Members.
         */
        Window mWindow{};

        // Storm canvas.
        bool mInitialized{false};

        unsigned mCanvasWidth = -1;
        unsigned mCanvasHeight = -1;

        array<XRenderColor, 4> mFlakeColors;

        int mStormItemCount = -1;

        float mFlakeRadiusX{0.0f};
        float mFlakeRadiusY{0.0f};

        float mWindowCornerWidth{0.0f};
        float mWindowCornerHeight{0.0f};

        // Wind.
        const float MAX_WIND_STRENGTH = 500.0f;
        const float INITIAL_WHIRL_STRENGTH = 150.0f;

        double mWhirlTimeSlider = 50.0;
        double mWhirlStrengthSlider = 300.0;

        double mWhirlStrength = 0.0;
        double mWhirlStartTime = 50.0;
        double mWhirlGustDuration = 50.0;

        bool   mPrevWindAvailable = false;
        double mPrevWindTime = 0.0;

        int    mWindEnabled = 0;
        int    mWindDirection = 0;
        float  mWindStrength = 100.0f;

        float  mCurrentWindX = 0.0f;
        double mStartNewWindTimer = 0.0;
        double mUpdateWindTimer = 0.0;

        // Storm canvas item vectors.
        vector<float> mFlakeVariation;
        vector<XRenderColor> mFlakeColor;
        vector<float> mFlakeRotation;
        vector<float> mSpinSpeed;
        vector<float> mFlakeMass;
        vector<float> mWindSensitivity;
        vector<float> mFlakeX;
        vector<float> mFlakeY;
        vector<float> mSpeedX;
        vector<float> mSpeedY;
        vector<float> mInitialSpeedY;

        // Vulkan Canvas Core Objects.
        VkInstance mInstance{VK_NULL_HANDLE};
        VkSurfaceKHR mSurface{VK_NULL_HANDLE};
        VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
        VkDevice mDevice{VK_NULL_HANDLE};
        VkQueue mGraphicsQueue{VK_NULL_HANDLE};
        int mGraphicsQueueFamilyIndex{-1};

        // Vulkan Canvas Swapchain & Pipeline.
        VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};
        vector<VkImage> mSwapchainImages;
        vector<VkImageView> mSwapchainImageViews;
        VkPipelineLayout mPipelineLayout{VK_NULL_HANDLE};
        VkPipeline mGraphicsPipeline{VK_NULL_HANDLE};
        VkShaderModule mVertShaderModule{VK_NULL_HANDLE};
        VkShaderModule mFragShaderModule{VK_NULL_HANDLE};

        // Vulkan Canvas Command Buffers & Sync.
        VkCommandPool mCommandPool{VK_NULL_HANDLE};
        VkCommandBuffer mCommandBuffer{VK_NULL_HANDLE};
        VkSemaphore mImageAvailableSemaphore{VK_NULL_HANDLE};
        VkSemaphore mRenderFinishedSemaphore{VK_NULL_HANDLE};
        VkFence mInFlightFence{VK_NULL_HANDLE};
};
