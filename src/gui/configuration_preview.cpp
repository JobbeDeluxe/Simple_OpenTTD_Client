#include "gui/configuration_preview.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

namespace sotc::ui {
namespace {

[[nodiscard]] std::size_t compute_field_width(const Section &section) {
    std::size_t width = 0;
    for (const auto &field : section.fields) {
        width = std::max(width, field.label.size());
    }
    if (width == 0) {
        return 0;
    }
    // Room for trailing colon and a space.
    return width + 2;
}

} // namespace

std::string render_sections(const std::vector<Section> &sections) {
    std::ostringstream stream;
    bool first_section = true;
    for (const auto &section : sections) {
        if (!first_section) {
            stream << '\n';
        }
        first_section = false;

        stream << "=== " << section.title << " ===\n";

        const auto field_width = compute_field_width(section);
        for (const auto &field : section.fields) {
            const std::string label = field.label + ':';
            stream << "  " << label;
            if (field_width > label.size()) {
                stream << std::string(field_width - label.size(), ' ');
            }
            stream << field.value << '\n';
        }

        if (!section.fields.empty() && !section.toggles.empty()) {
            stream << '\n';
        }

        for (const auto &toggle : section.toggles) {
            stream << "  [" << (toggle.enabled ? 'x' : ' ') << "] " << toggle.label;
            if (!toggle.hint.empty()) {
                stream << " - " << toggle.hint;
            }
            stream << '\n';
        }

        if (!section.notes.empty()) {
            if (!section.fields.empty() || !section.toggles.empty()) {
                stream << '\n';
            }
            for (const auto &note : section.notes) {
                stream << "  - " << note << '\n';
            }
        }
    }

    return stream.str();
}

} // namespace sotc::ui
