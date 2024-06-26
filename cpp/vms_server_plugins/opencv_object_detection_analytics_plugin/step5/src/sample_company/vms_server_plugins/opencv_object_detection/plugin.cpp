// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "plugin.h"

#include "nx/sdk/analytics/helpers/plugin.h" // Add missing include path

#include "engine.h"

#include <curl/curl.h>
#include "kafka_consumer.h"
#include <iostream>

namespace sample_company {
namespace vms_server_plugins {
namespace opencv_object_detection {

using namespace nx::sdk;
using namespace nx::sdk::analytics;

Result<IEngine*> Plugin::doObtainEngine()
{
    const auto utilityProvider = this->utilityProvider();
    const std::filesystem::path pluginHomeDir = utilityProvider->homeDir();
    auto engine = new Engine(pluginHomeDir);

    //Get the NX Camera ID
    std::string nxCamId = "fb80b2f7-23d1-7f74-94a8-1aeec6de601b";

    // Start instance segmentation via Flask app
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
    // Initialize Kafka consumer and set up the detection callbacks
    KafkaConsumer consumer("localhost:9092", "color_detection");
    consumer.setMessageCallback([engine](const std::string& message) {
        engine->processColorDetection(message);
    });
    consumer.start();

    return engine;

}

/**
 * JSON with the particular structure. Note that it is possible to fill in the values that are not
 * known at compile time.
 *
 * - id: Unique identifier for a plugin with format "{vendor_id}.{plugin_id}", where
 *     {vendor_id} is the unique identifier of the plugin creator (person or company name) and
 *     {plugin_id} is the unique (for a specific vendor) identifier of the plugin.
 * - name: A human-readable short name of the plugin (displayed in the "Camera Settings" window
 *     of the Client).
 * - description: Description of the plugin in a few sentences.
 * - version: Version of the plugin.
 * - vendor: Plugin creator (person or company) name.
 */
std::string Plugin::manifestString() const
{
    return /*suppress newline*/ 1 + R"json(
{
    "id": "sample.opencv_object_detection",
    "name": "OpenCV object detection",
    "description": ")json"
        "This plugin is for object detection and color detection. It's based on OpenCV."
        R"json(",
    "version": "1.0.0",
    "vendor": "Sample Inc."
}
)json";
}

/**
 * Called by the Server to instantiate the Plugin object.
 *
 * The Server requires the function to have C linkage, which leads to no C++ name mangling in the
 * export table of the plugin dynamic library, so that makes it possible to write plugins in any
 * language and compiler.
 *
 * NX_PLUGIN_API is the macro defined by CMake scripts for exporting the function.
 */
extern "C" NX_PLUGIN_API nx::sdk::IPlugin* createNxPlugin()
{
    // The object will be freed when the Server calls releaseRef().
    return new Plugin();
}

} // namespace opencv_object_detection
} // namespace vms_server_plugins
} // namespace sample_company
