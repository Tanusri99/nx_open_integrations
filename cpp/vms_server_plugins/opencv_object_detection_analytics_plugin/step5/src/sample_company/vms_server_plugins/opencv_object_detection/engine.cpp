// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "engine.h"

#include "device_agent.h"
#include "color_detector.h"
#include <nlohmann/json.hpp> // For JSON.

namespace sample_company {
namespace vms_server_plugins {
namespace opencv_object_detection {

using namespace nx::sdk;
using namespace nx::sdk::analytics;

Engine::Engine(std::filesystem::path pluginHomeDir):
    // Call the DeviceAgent helper class constructor telling it to verbosely report to stderr.
    nx::sdk::analytics::Engine(/*enableOutput*/ true),
    m_pluginHomeDir(pluginHomeDir)
{
}

Engine::~Engine()
{
}

/**
 * Called when the Server opens a video-connection to the camera if the plugin is enabled for this
 * camera.
 *
 * @param outResult The pointer to the structure which needs to be filled with the resulting value
 *     or the error information.
 * @param deviceInfo Contains various information about the related device such as its id, vendor,
 *     model, etc.
 */
void Engine::doObtainDeviceAgent(Result<IDeviceAgent*>* outResult, const IDeviceInfo* deviceInfo)
{
    *outResult = new DeviceAgent(deviceInfo, m_pluginHomeDir);
}

void Engine::processColorDetection(const std::string& colorMessage)
{
    nlohmann::json message = nlohmann::json::parse(colorMessage);
    std::string label = message["label"];
    float confidence = message["confidence"];
    std::string topColors = message["top_colors"];
    std::string pantsColors = message["pants_colors"];
    std::string dominantColors = message["dominant_colors"];

    // Handle the detection results based on your logic
    if (label == "person") {
        // Process top and pants color detection
        std::cout << "Detected person with top color: " << topColors << " and pants color: " << pantsColors << std::endl;
    } else {
        // Process other detections
        std::cout << "Detected " << label << " with dominant color: " << dominantColors << std::endl;
    }

}

Result<IEngine*> Plugin::doObtainEngine()
{
    const auto utilityProvider = this->utilityProvider();
    const std::filesystem::path pluginHomeDir = utilityProvider->homeDir();
    auto engine = new Engine(pluginHomeDir);

    std::string nxCamId = "your-nx-cam-id"; // Retrieve the actual camera ID dynamically if needed

    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string jsonPayload = "{\"nx_cam_id\": \"" + nxCamId + "\"}";
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/start_detection");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    KafkaConsumer consumer("localhost:9092", "color_detection");
    consumer.setMessageCallback([engine](const std::string& message) {
        engine->processColorDetection(message);
    });
    consumer.start();

    return engine;
}

/**
 *  @return JSON with the particular structure. Note that it is possible to fill in the values
 * that are not known at compile time, but should not depend on the Engine settings.
 */
std::string Plugin::manifestString() const
{
    return /*suppress newline*/ 1 + R"json(
    {
        "id": "sample.opencv_object_detection",
        "name": "OpenCV object detection",
        "description": "This plugin is for object detection and color detection using OpenCV.",
        "version": "1.1.0",
        "vendor": "Sample Inc."
    }
    )json";
}

extern "C" NX_PLUGIN_API nx::sdk::IPlugin* createNxPlugin()
{
    return new Plugin();
}

} // namespace opencv_object_detection
} // namespace vms_server_plugins
} // namespace sample_company
