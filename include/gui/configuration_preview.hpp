#pragma once

#include <string>
#include <vector>

namespace sotc::ui {

struct FieldLine {
    std::string label;
    std::string value;
};

struct ToggleLine {
    std::string label;
    bool enabled{false};
    std::string hint;
};

struct Section {
    std::string title;
    std::vector<FieldLine> fields;
    std::vector<ToggleLine> toggles;
    std::vector<std::string> notes;
};

[[nodiscard]] std::string render_sections(const std::vector<Section> &sections);

} // namespace sotc::ui
