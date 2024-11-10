

#include <fmt/core.h>
namespace arm_face_id {
namespace log {
template <typename... Args>
void info(fmt::format_string<Args...> msg, Args&&... args) {
  SPDLOG_INFO(msg, args...);
}

template <typename... Args>
void warn(fmt::format_string<Args...> msg, Args&&... args) {
  SPDLOG_WARN(msg, args...);
}

template <typename... Args>
void debug(fmt::format_string<Args...> msg, Args&&... args) {
  SPDLOG_DEBUG(msg, args...);
}

template <typename... Args>
void error(fmt::format_string<Args...> msg, Args&&... args) {
  SPDLOG_ERROR(msg, args...);
}
}  // namespace log
}  // namespace arm_face_id