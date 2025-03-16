# Seytan128 Hash Function - Internal Technical Documentation

## Overview

Seytan128 is a 128-bit hash function based on polynomial accumulation and multiplication by a constant derived from the golden ratio. This document explains the mathematical principles, security properties, and implementation details for internal reference.

## Mathematical Foundation

### String-to-Integer Conversion

The function converts an input string into a 128-bit number using base-31 polynomial accumulation:

```
x = s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]*31^0
```

Where:
- `s[i]` is the ASCII value of the i-th character
- `n` is the length of the string

This is equivalent to Horner's method:
```
x = 0
for each character c in string:
    x = x*31 + ASCII(c)
```

### Hash Computation

The 128-bit hash is calculated as:
```
H(x) = (x * K) mod 2^128
```

Where:
- `x` is the 128-bit integer from the polynomial accumulation
- `K` is the constant (golden ratio based) 0x9e3779b97f4a7c159e3779b97f4a7c15
- Modulo 2^128 happens naturally due to integer overflow

## Mathematical Properties

1. **Linearity**: The hash function is linear, meaning:
   ```
   H(a + b) = H(a) + H(b) mod 2^128
   ```

2. **Multiplicative Relationship**: For any input `x` and constant `c`:
   ```
   H(c*x) = c*H(x) mod 2^128
   ```

3. **Collision Structure**: Inputs that differ by multiples of 2^128/K will collide:
   ```
   If (x1 - x2) * K ≡ 0 (mod 2^128), then H(x1) = H(x2)
   ```

4. **Weak Avalanche**: Small input changes typically affect only a portion of output bits, with predictable propagation patterns.

## Cryptographic Security Analysis

### Known Weaknesses

1. **Reversibility**: The function is invertible when K has a multiplicative inverse mod 2^128.
   - Given H(x) and K, computing x = H(x) * K^(-1) mod 2^128 is possible.

2. **Collision Finding**: Collisions can be found by solving:
   ```
   (x1 - x2) * K ≡ 0 (mod 2^128)
   ```
   This is equivalent to finding multiples of 2^128/gcd(K, 2^128).

3. **Poor Diffusion**: Bit changes in input don't propagate well across all output bits.

4. **Extension Vulnerability**: Length extensions are trivial due to the polynomial structure.

### Expected Security Metrics

- **Collision Resistance**: ~2^64 operations (instead of 2^64 for ideal 128-bit hash)
- **Preimage Resistance**: Not assured due to linearity
- **Avalanche Effect**: 40-50% is expected (rather than ~50% for ideal hashes)

## Enhanced Version Improvements

The enhanced version adds:

1. **Non-Linearity**: Through bit rotations and XOR operations
2. **Multiple Rounds**: Increasing diffusion and making analysis harder
3. **Mixing High/Low Bits**: Improving avalanche effects across the full 128 bits
4. **Length Incorporation**: Preventing length extension attacks

## Performance Considerations

- Original version is very fast (one multiplication operation)
- Enhanced version is slower but provides significantly better security properties
- Both versions are faster than SHA-2/SHA-3 but with reduced security guarantees

## Testing Methodology

1. **Avalanche Testing**: Measures bit change propagation (ideally ~50%)
2. **Collision Testing**: Random input pairs looking for matching outputs
3. **Dieharder**: Statistical randomness testing suite for output distributions

## Not For Production Use

This hash function is not suitable for:
- Password hashing
- Digital signatures
- Message authentication codes
- Any security-critical applications

## Mathematical Proof of Weakness

Let K be our constant. We can find x₁≠x₂ such that H(x₁)=H(x₂) by solving:

x₁·K ≡ x₂·K (mod 2^128)

This means (x₁-x₂)·K ≡ 0 (mod 2^128)

Since K is odd (due to the golden ratio derivation), gcd(K,2^128)=1, meaning we can find values of x₁,x₂ that differ by 2^128/gcd(K,2^128) = 2^128.

This implies that any inputs differing by exact multiples of 2^128/K will produce identical hash values, providing a direct avenue for generating collisions.
