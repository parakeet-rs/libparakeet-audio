#pragma once

#include <cstdint>
#include <span>

#include "AudioTypes.h"

namespace parakeet_audio {
constexpr size_t kAudioTypeSniffBufferSize = 4096;

AudioType DetectAudioType(const uint8_t* buf, std::size_t len);

inline AudioType DetectAudioType(const std::span<const uint8_t>& data) {
  return DetectAudioType(data.data(), data.size());
}

inline bool IsAudioBufferRecognised(const uint8_t* buf, std::size_t len) {
  return DetectAudioType(buf, len) != AudioType::kUnknownType;
}

inline bool IsAudioBufferRecognised(const std::span<const uint8_t>& data) {
  return IsAudioBufferRecognised(data.data(), data.size());
}

inline const char* DetectAudioExtension(const uint8_t* buf, std::size_t len) {
  return GetAudioTypeExtension(DetectAudioType(buf, len));
}

}  // namespace parakeet_audio
