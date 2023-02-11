#pragma once

#include <cstddef>
#include <cstdint>

#include <span>

namespace parakeet_audio {
/**
 * @brief Get the ID3 header size.
 * @private
 *
 * @param buffer
 * @param buffer_len
 * @return std::size_t
 */
std::size_t GetID3HeaderSize(const uint8_t* buffer, size_t buffer_len);

/**
 * @brief Get APEv2 header size
 * @private
 *
 * @param buffer
 * @param buffer_len
 * @return std::size_t
 */
std::size_t GetAPEv2FullSize(const uint8_t* buffer, size_t buffer_len);

/**
 * @brief Detect ID3v2/APEv2 Tag size.
 *        When detected, a positive integer indicating its size from `buf`
 *        will be returned.
 *
 * @param buffer
 * @param buffer_len
 * @return std::size_t 0 if header metatype is unsupported.
 */
std::size_t GetAudioHeaderMetadataSize(const uint8_t* buffer, size_t buffer_len);

}  // namespace parakeet_audio
