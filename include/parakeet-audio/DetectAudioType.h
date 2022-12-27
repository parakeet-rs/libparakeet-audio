#pragma once

#include "AudioTypes.h"

#include <cstdint>
#include <span>

namespace parakeet_audio {
constexpr size_t kAudioTypeSniffBufferSize = 4096;

AudioType DetectAudioType(const std::span<const uint8_t> buffer);

inline auto IsAudioBufferRecognised(const std::span<const uint8_t> buffer) -> bool {
  return DetectAudioType(buffer) != AudioType::kUnknownType;
}

inline auto DetectAudioExtension(const std::span<const uint8_t> buffer) -> const char* {
  return GetAudioTypeExtension(DetectAudioType(buffer));
}

}  // namespace parakeet_audio
