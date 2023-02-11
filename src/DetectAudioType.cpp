#include <cstdint>
#include "parakeet_endian.h"

#include "AudioMetadata.h"
#include "parakeet-audio/DetectAudioType.h"

namespace parakeet_audio {

inline bool is_mp3(uint32_t magic) {
  // Frame-sync, should have first 11-bits set to 1.
  constexpr uint32_t kMP3AndMasks = 0b1111'1111'1110'0000U << 16;
  constexpr uint32_t kMP3Expected = 0b1111'1111'1110'0000U << 16;
  return ((magic & kMP3AndMasks) == kMP3Expected);
}

inline bool is_aac(uint32_t magic) {
  // Frame-sync, should have first 12-bits set to 1.
  constexpr uint32_t kAacAndMasks = 0b1111'1111'1111'0110 << 16;
  constexpr uint32_t kAacExpected = 0b1111'1111'1111'0000 << 16;

  return ((magic & kAacAndMasks) == kAacExpected);
}

// NOLINTBEGIN(bugprone-reserved-identifier)
constexpr uint32_t kMagic_fLaC = 0x66'4c'61'43U;  // Free Lossless Audio Codec (FLAC)
constexpr uint32_t kMagic_OggS = 0x4F'67'67'53U;  // Ogg
constexpr uint32_t kMagic_FRM8 = 0x46'52'4D'38U;  // Direct Stream Digital (DSDIFF)
constexpr uint32_t kMagic_ftyp = 0x66'74'79'70U;  // MP4 Frame
constexpr uint32_t kMagic__wma = 0x30'26'B2'75U;  // Windows WMA/WMV/ASF
constexpr uint32_t kMagic_RIFF = 0x52'49'46'46U;  // Waveform Audio File Format (WAV)
constexpr uint32_t kMagic__MAC = 0x4D'41'43'20U;  // Monkey's Audio (APE; uint8_t "MAC ")

constexpr uint32_t kMagic_ftyp_MSNV = 0x4d'53'4e'56U;  // MPEG-4 (.MP4) for SonyPSP
constexpr uint32_t kMagic_ftyp_NDAS = 0x4e'44'41'53U;  // Nero Digital AAC Audio
constexpr uint32_t kMagic_ftyp_isom = 0x69'73'6F'6DU;  // isom - MP4 (audio only?)
constexpr uint32_t kMagic_ftyp_iso2 = 0x69'73'6F'32U;  // iso2 - MP4 (audio only?)

constexpr uint32_t kMagic_ftyp_M4A = 0x4d'34'41U;  // iTunes AAC-LC (.M4A) Audio
constexpr uint32_t kMagic_ftyp_M4B = 0x4d'34'42U;  // iTunes AAC-LC (.M4B) Audio Book
constexpr uint32_t kMagic_ftyp_mp4 = 0x6D'70'34U;  // MP4 container, used by QQ Music (E-AC-3 JOC)
// NOLINTEND(bugprone-reserved-identifier)

AudioType DetectAudioType(const uint8_t* buffer, size_t buffer_len) {
  // Seek optional id3 tag.
  if (auto meta_len = GetAudioHeaderMetadataSize(buffer, buffer_len); meta_len > 0) {
    if (meta_len > buffer_len) {
      return AudioType::kUnknownType;
    }

    buffer += meta_len;
    buffer_len -= meta_len;
  }

  // Check 4 byte magic header
  if (buffer_len >= sizeof(uint32_t)) {
    auto magic = ReadBigEndian<uint32_t>(buffer);

    switch (magic) {
      case kMagic_fLaC:
        return AudioType::kAudioTypeFLAC;
      case kMagic_OggS:
        return AudioType::kAudioTypeOGG;
      case kMagic_FRM8:
        return AudioType::kAudioTypeDFF;
      case kMagic__wma:
        return AudioType::kAudioTypeWMA;
      case kMagic_RIFF:
        return AudioType::kAudioTypeWAV;
      case kMagic__MAC:
        return AudioType::kAudioTypeAPE;
      default:
        break;
    }

    // Detect type by its frame header
    if (is_aac(magic)) {
      return AudioType::kAudioTypeAAC;
    } else if (is_mp3(magic)) {
      return AudioType::kAudioTypeMP3;
    }
  }

  // Check MP4 container.
  constexpr std::size_t kMP4DetectMinLen = 0x10;
  constexpr std::size_t kMP4OffsetFtypFieldKey = 0x04;
  constexpr std::size_t kMP4OffsetFtypFieldValue = 0x08;
  constexpr std::size_t kShiftRemoveLastByte = 0x08;
  if (buffer_len >= kMP4DetectMinLen && ReadBigEndian<uint32_t>(&buffer[kMP4OffsetFtypFieldKey]) == kMagic_ftyp) {
    auto magic = ReadBigEndian<uint32_t>(&buffer[kMP4OffsetFtypFieldValue]);

    switch (magic) {
      case kMagic_ftyp_isom:
      case kMagic_ftyp_iso2:
      case kMagic_ftyp_MSNV:
        return AudioType::kAudioTypeMP4;

      case kMagic_ftyp_NDAS:
        return AudioType::kAudioTypeM4A;

      default: {
        // Do nothing
      }
    }

    // Check only first 3 bytes.
    switch (magic >> kShiftRemoveLastByte) {
      case kMagic_ftyp_M4A:
        return AudioType::kAudioTypeM4A;
      case kMagic_ftyp_M4B:
        return AudioType::kAudioTypeM4B;
      case kMagic_ftyp_mp4:
        return AudioType::kAudioTypeMP4;

      default: {
        // Do nothing
      }
    }
  }

  return AudioType::kUnknownType;
}

}  // namespace parakeet_audio
