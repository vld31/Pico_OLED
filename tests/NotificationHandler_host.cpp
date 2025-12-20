#include "NotificationHandler.h"
#include <string>

// super-small parser:
// supports either:
//   {"message":"Hello"}
// or
//   [{"title":"Hello","body":"World"}]
static std::string extract_field(const std::string& s, const char* key) {
    std::string k = std::string("\"") + key + "\"";
    auto pos = s.find(k);
    if (pos == std::string::npos) return {};
    pos = s.find(':', pos);
    if (pos == std::string::npos) return {};
    pos++;
    while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\t')) pos++;
    if (pos >= s.size() || s[pos] != '"') return {};
    pos++;
    auto end = s.find('"', pos);
    if (end == std::string::npos) return {};
    return s.substr(pos, end - pos);
}

void NotificationHandler::processMessage(const std::string& message) {
    display_.clear();
    display_.drawText(0, 0, "Notification:");
    display_.drawText(0, 1, message);
    display_.show();

    if (sound_) sound_->beep(1000, 200);
}

void NotificationHandler::poll() {
    std::string body;
    // URL doesn't matter for unit tests, but we want it non-empty:
    if (!wifi_.get("/notifications", body)) return;

    // try {"message":"X"}
    auto msg = extract_field(body, "message");
    if (!msg.empty()) {
        processMessage(msg);
        return;
    }

    // try [{"title":"T","body":"B"}]
    auto title = extract_field(body, "title");
    auto b = extract_field(body, "body");

    if (!title.empty() || !b.empty()) {
        display_.clear();
        if (!title.empty()) display_.drawText(0, 0, title);
        if (!b.empty()) display_.drawText(0, 1, b);
        display_.show();
        if (sound_) sound_->beep(1000, 200);
    }
}
