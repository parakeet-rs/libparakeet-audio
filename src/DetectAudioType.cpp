#include "parakeet_endian.h"

#include "AudioMetadata.h"
#include "parakeet-audio/DetectAudioType.h"

namespace parakeet_audio {

inline auto is_mp3(uint32_t magic) -> bool {
  // Frame-sync, should have first 11-bits set to 1.
  constexpr uint32_t kMP3AndMasks = 0b1111'1111'1110'0000u << 16;
  constexpr uint32_t kMP3Expected = 0b1111'1111'1110'0000u << 16;
  return ((magic & kMP3AndMasks) == kMP3Expected);
}

inline auto is_aac(uint32_t magic) -> bool {
  // Frame-sync, should have first 12-bits set to 1.
  constexpr uint32_t kAacAndMasks = 0b1111'1111'1111'0110 << 16;
  constexpr uint32_t kAacExpected = 0b1111'1111'1111'0000 << 16;

  return ((magic & kAacAndMasks) == kAacExpected);
}

constexpr uint32_t kMagic_fLaC = 0x66'4c'61'43u;  // Free Lossless Audio Codec (FLAC)
constexpr uint32_t kMagic_OggS = 0x4F'67'67'53u;  // Ogg
constexpr uint32_t kMagic_FRM8 = 0x46'52'4D'38u;  // Direct Stream Digital (DSDIFF)
constexpr uint32_t kMagic_ftyp = 0x66'74'79'70u;  // MP4 Frame
constexpr uint32_t kMagic__wma = 0x30'26'B2'75u;  // Windows WMA/WMV/ASF
constexpr uint32_t kMagic_RIFF = 0x52'49'46'46u;  // Waveform Audio File Format (WAV)
constexpr uint32_t kMagic__MAC = 0x4D'41'43'20u;  // Monkey's Audio (APE; uint8_t "MAC ")

constexpr uint32_t kMagic_ftyp_MSNV = 0x4d'53'4e'56u;  // MPEG-4 (.MP4) for SonyPSP
constexpr uint32_t kMagic_ftyp_NDAS = 0x4e'44'41'53u;  // Nero Digital AAC Audio
constexpr uint32_t kMagic_ftyp_isom = 0x69'73'6F'6Du;  // isom - MP4 (audio only?)
constexpr uint32_t kMagic_ftyp_iso2 = 0x69'73'6F'32u;  // iso2 - MP4 (audio only?)

constexpr uint32_t kMagic_ftyp_M4A = 0x4d'34'41u;  // iTunes AAC-LC (.M4A) Audio
constexpr uint32_t kMagic_ftyp_M4B = 0x4d'34'42u;  // iTunes AAC-LC (.M4B) Audio Book
constexpr uint32_t kMagic_ftyp_mp4 = 0x6D'70'34u;  // MP4 container, used by QQ Music (E-AC-3 JOC)

auto DetectAudioType(const std::span<const uint8_t> buffer) -> AudioType {
  using enum AudioType;
  auto buf = buffer;

  // Seek optional id3 tag.
  if (auto meta_len = GetAudioHeaderMetadataSize(buffer); meta_len > 0) {
    if (meta_len > buffer.size()) {
      return kUnknownType;
    }

    buf = std::span{&buf[meta_len], buf.size() - meta_len};
  }

  // Check 4 byte magic header
  if (buf.size() >= sizeof(uint32_t)) {
    auto magic = ReadBigEndian<uint32_t>(&buf[0]);

    switch (magic) {
      case kMagic_fLaC:
        return kAudioTypeFLAC;
      case kMagic_OggS:
        return kAudioTypeOGG;
      case kMagic_FRM8:
        return kAudioTypeDFF;
      case kMagic__wma:
        return kAudioTypeWMA;
      case kMagic_RIFF:
        return kAudioTypeWAV;
      case kMagic__MAC:
        return kAudioTypeAPE;
      default: {
        // Do nothing
      }
    }

    // Detect type by its frame header
    if (is_aac(magic)) {
      return kAudioTypeAAC;
    } else if (is_mp3(magic)) {
      return kAudioTypeMP3;
    }
  }

  // Check MP4 container.
  constexpr std::size_t kMP4DetectMinLen = 0x10;
  constexpr std::size_t kMP4OffsetFtypFieldKey = 0x04;
  constexpr std::size_t kMP4OffsetFtypFieldValue = 0x08;
  constexpr std::size_t kShiftRemoveLastByte = 0x08;
  if (buf.size() >= kMP4DetectMinLen && ReadBigEndian<uint32_t>(&buf[kMP4OffsetFtypFieldKey]) == kMagic_ftyp) {
    auto magic = ReadBigEndian<uint32_t>(&buf[kMP4OffsetFtypFieldValue]);

    switch (magic) {
      case kMagic_ftyp_isom:
      case kMagic_ftyp_iso2:
      case kMagic_ftyp_MSNV:
        return kAudioTypeMP4;

      case kMagic_ftyp_NDAS:
        return kAudioTypeM4A;

      default: {
        // Do nothing
      }
    }

    // Check only first 3 bytes.
    switch (magic >> kShiftRemoveLastByte) {
      case kMagic_ftyp_M4A:
        return kAudioTypeM4A;
      case kMagic_ftyp_M4B:
        return kAudioTypeM4B;
      case kMagic_ftyp_mp4:
        return kAudioTypeMP4;

      default: {
        // Do nothing
      }
    }
  }

  return kUnknownType;
}

}  // namespace parakeet_audio
