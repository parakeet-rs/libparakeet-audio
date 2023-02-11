#pragma once

#include "AudioTypes.h"

#include <cstddef>
#include <cstdint>

namespace parakeet_audio {
constexpr size_t kAudioTypeSniffBufferSize = 4096;

AudioType DetectAudioType(const uint8_t* buffer, size_t buffer_len);

inline bool IsAudioBufferRecognised(const uint8_t* buffer, size_t buffer_len) {
  return DetectAudioType(buffer, buffer_len) != AudioType::kUnknownType;
}

inline const char* DetectAudioExtension(const uint8_t* buffer, size_t buffer_len) {
  return GetAudioTypeExtension(DetectAudioType(buffer, buffer_len));
}

}  // namespace parakeet_audio
