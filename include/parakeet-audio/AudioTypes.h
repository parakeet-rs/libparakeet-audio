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
  kAudioTypeWMA = 7,  // While possible, it is rare to find a lossless WMA file.

  // Lossless
  kAudioTypeFLAC = kAudioTypeMaskLossless | 1,
  kAudioTypeDFF = kAudioTypeMaskLossless | 2,
  kAudioTypeWAV = kAudioTypeMaskLossless | 3,
  kAudioTypeAPE = kAudioTypeMaskLossless | 5,
};

inline const char* GetAudioTypeExtension(AudioType type) {
  switch (type) {
      // Lossy types
    case AudioType::kAudioTypeOGG:
      return "ogg";
    case AudioType::kAudioTypeAAC:
      return "aac";
    case AudioType::kAudioTypeMP3:
      return "mp3";
    case AudioType::kAudioTypeM4A:
      return "m4a";
    case AudioType::kAudioTypeM4B:
      return "m4b";
    case AudioType::kAudioTypeMP4:
      return "mp4";

      // Lossless types
    case AudioType::kAudioTypeFLAC:
      return "flac";
    case AudioType::kAudioTypeDFF:
      return "dff";
    case AudioType::kAudioTypeWAV:
      return "wav";
    case AudioType::kAudioTypeWMA:
      return "wma";
    case AudioType::kAudioTypeAPE:
      return "ape";

    default:
      return "bin";
  }
}

inline bool AudioIsLossless(AudioType type) {
  return (static_cast<uint32_t>(type) & kAudioTypeMaskLossless) != 0;
}

}  // namespace parakeet_audio
