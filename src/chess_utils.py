import numpy as np


def u64(x: int) -> np.uint64:
    return np.uint64(x & 0xFFFFFFFFFFFFFFFF)


def bitscan(bitboard: np.uint64) -> list[int]:
    """
    Returns a list of indices (0–63) of the bits set in the bitboard.
    """
    result = []
    b = int(bitboard)  # Convert to Python int for bit manipulation
    while b:
        lsb = b & -b  # isolate least significant bit
        index = lsb.bit_length() - 1  # equivalent to log2(lsb)
        result.append(index)
        b ^= lsb  # remove the bit
    return result


def fr_to_bit(file, rank):
    return rank * 8 + file


def bit_to_fr(bit):
    return (bit % 8, bit // 8)
