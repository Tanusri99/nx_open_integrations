// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "detection.h"

namespace sample_company {
namespace vms_server_plugins {
namespace opencv_object_detection {

// Class labels for the MobileNet SSD model (VOC dataset).
const std::vector<std::string> kClasses{
    "background", "aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat",
    "chair", "cow", "dining table", "dog", "horse", "motorbike", "person", "potted plant",
    "sheep", "sofa", "train", "tv monitor"
};
const std::vector<std::string> kClassesToDetect{"cat", "dog", "person"};
const std::map<std::string, std::string> kClassesToDetectPluralCapitalized{
    {"cat", "Cats"}, {"dog", "Dogs"}, {"person", "People"}};

void Engine::processColorDetection(const std::string& message)
{
    // Parse the message and extract the color
    // For now, we will just assume that the color is "red"
    const std::string color = "red";

    // Create a detection object
    auto detection = std::make_shared<Detection>(
        Detection{nx::sdk::analytics::Rect(), "person", 0.9f, nx::sdk::Uuid(), color});

    // Add the detection to the list
    m_detections.push_back(detection);
}

} // namespace opencv_object_detection
} // namespace vms_server_plugins
} // namespace sample_company

} // namespace opencv_object_detection
} // namespace vms_server_plugins
} // namespace sample_company
