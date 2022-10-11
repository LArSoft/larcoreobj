#ifndef RAW_TYPES_H
#define RAW_TYPES_H

#include <limits> // std::numeric_limits<>

namespace raw {

  typedef enum _compress {
    kNone,            ///< no compression
    kHuffman,         ///< Huffman Encoding
    kZeroSuppression, ///< Zero Suppression algorithm
    kZeroHuffman,     ///< Zero Suppression followed by Huffman Encoding
    kDynamicDec,      ///< Dynamic decimation
    kFibonacci        ///< Fibonacci encoding
  } Compress_t;

  typedef enum _auxdettype {
    kUnknownAuxDet, ///< no idea
    kScintillator,  ///< Scintillator paddle
    kTimeOfFlight,  ///< Time of flight
    kCherenkov      ///< Cherenkov counter
  } AuxDetType_t;

  /// Type representing a TDC tick
  typedef int TDCtick_t;

  /// Type representing the ID of a readout channel
  typedef unsigned int ChannelID_t;

  /// ID of an invalid channel
  constexpr ChannelID_t InvalidChannelID = std::numeric_limits<ChannelID_t>::max();

  /// Returns whether the specified channel ID is valid
  /// @note This does not mean that channel exists in the current geometry.
  inline constexpr bool isValidChannelID(raw::ChannelID_t channel)
  {
    return channel != InvalidChannelID;
  }

} // namespace raw

#endif
