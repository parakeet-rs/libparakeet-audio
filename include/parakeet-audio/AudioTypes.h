#pragma once

#include <cstdint>

namespace parakeet_audio {
constexpr uint32_t kAudioTypeMaskLossless = 1 << 5;

enum class AudioType : uint32_t {
  kUnknownType = 0,

  // Lossy
  kAudioTypeOGG = 1,
  kAudioTypeAAC = 2,
  kAudioTypeMP3 = 3,
  kAudioTypeM4A = 4,
  kAudioTypeM4B = 5,
  kAudioTypeMP4 = 6,

  // Lossless
  kAudioTypeFLAC = kAudioTypeMaskLossless | 1,
  kAudioTypeDFF = kAudioTypeMaskLossless | 2,
  kAudioTypeWAV = kAudioTypeMaskLossless | 3,
  kAudioTypeWMA = kAudioTypeMaskLossless | 4,
  kAudioTypeAPE = kAudioTypeMaskLossless | 5,
};

inline const char* GetAudioTypeExtension(AudioType type) {
  using enum AudioType;
  switch (type) {
      // Lossy types
    case kAudioTypeOGG:
      return "ogg";
    case kAudioTypeAAC:
      return "aac";
    case kAudioTypeMP3:
      return "mp3";
    case kAudioTypeM4A:
      return "m4a";
    case kAudioTypeM4B:
      return "m4b";
    case kAudioTypeMP4:
      return "mp4";

      // Lossless types
    case kAudioTypeFLAC:
      return "flac";
    case kAudioTypeDFF:
      return "dff";
    case kAudioTypeWAV:
      return "wav";
    case kAudioTypeWMA:
      return "wma";
    case kAudioTypeAPE:
      return "ape";

    default:
      return "bin";
  }
}

inline bool AudioIsLossless(AudioType type) {
  return (static_cast<uint32_t>(type) & kAudioTypeMaskLossless) != 0;
}

}  // namespace parakeet_audio
