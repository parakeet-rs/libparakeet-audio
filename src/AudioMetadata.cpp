#include "AudioMetadata.h"
#include "endian.h"

namespace parakeet_audio {

inline int32_t ParseID3SyncSafeInt(const uint8_t* p) {
  auto a = p[0];
  auto b = p[1];
  auto c = p[2];
  auto d = p[3];

  // Sync safe int should use only lower 7-bits of each byte.
  if ((a | b | c | d) & 0x80) {
    return 0;
  }

  int32_t result = 0;
  result |= int32_t{a} << (3 * 8 - 3);
  result |= int32_t{b} << (2 * 8 - 2);
  result |= int32_t{c} << (1 * 8 - 1);
  result |= int32_t{d} << (0 * 8 - 0);
  return result;
}

std::size_t GetID3HeaderSize(const uint8_t* buf, std::size_t len) {
  constexpr uint32_t kID3v1Masks = 0xFF'FF'FF'00u;  // Select first 3 bytes
  constexpr uint32_t kID3v1Value = 0x54'41'47'00u;  // 'TAG\x00'

  constexpr uint32_t kID3v2Masks = 0xFF'FF'FF'00u;  // Select first 3 bytes
  constexpr uint32_t kID3v2Value = 0x49'44'33'00u;  // 'ID3\x00'

  if (len < 10) {
    return 0;
  }

  const auto magic = parakeet_audio::ReadBigEndian<uint32_t>(&buf[0]);

  // ID3v1 and ID3v1.1: flat 128 bytes
  if ((magic & kID3v1Masks) == kID3v1Value) {
    return 128;
  }

  if ((magic & kID3v2Masks) == kID3v2Value) {
    // offset    value
    //      0    header('ID3')
    //      3    uint8_t(ver_major) uint8_t(ver_minor)
    //      5    uint8_t(flags)
    //      6    uint32_t(inner_tag_size)
    //     10    byte[inner_tag_size] id3v2 data
    //     ??    byte[*] original_file_content
    const auto id3v2InnerTagSize = ParseID3SyncSafeInt(&buf[6]);
    return 10 + id3v2InnerTagSize;
  }

  return 0;
}

std::size_t GetAPEv2FullSize(const uint8_t* buf, std::size_t len) {
  constexpr uint64_t kAPEv2Magic = 0x41'50'45'54'41'47'45'58u;  // 'APETAGEX'
  constexpr std::size_t kAPEv2HeaderSize = 32;

  auto magic = parakeet_audio::ReadBigEndian<uint64_t>(&buf[0]);
  if (kAPEv2Magic == magic) {
    // Tag size in bytes including footer and all tag items excluding the header.
    return parakeet_audio::ReadLittleEndian<uint32_t>(&buf[0x0c]) + kAPEv2HeaderSize;
  }

  return 0;
}

std::size_t GetAudioHeaderMetadataSize(const uint8_t* buf, std::size_t len) {
  // Not enough bytes to detect
  if (len < 10) {
    return 0;
  }

  std::size_t id3_meta_size = GetID3HeaderSize(buf, len);
  if (id3_meta_size) {
    return id3_meta_size;
  }

  // It's possible to have APEv2 header at the beginning of a file, though rare.
  std::size_t ape_meta_size = GetAPEv2FullSize(buf, len);
  if (ape_meta_size) {
    return ape_meta_size;
  }

  return 0;
}

}  // namespace parakeet_audio
