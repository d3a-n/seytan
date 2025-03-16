# seytan256 Hash: Mathematical and Technical Documentation

---

## 1. Mathematical Explanation

The seytan256 hash function \( H(S) \) computes a unique 256-bit hash for an input string \( S \) composed of characters \( s_0, s_1, \dots, s_{n-1} \), following the formula:

![formula](formula.png)

In LaTeX format:
\[
H(S) = \left(\sum_{i=0}^{n-1} \left( \text{val}(s_i) \times (i + 1) \times K \oplus R \right)\right) \mod P
\]

Where:
- \(\text{val}(s_i)\): ASCII value of character \( s_i \).
- \((i + 1)\): Introduces position-dependent weighting, ensuring sensitivity to input order.
- \(K\): A fixed 256-bit constant derived from the golden ratio for thorough bit mixing.
- \(R\): A secondary 256-bit constant applied via XOR to increase diffusion and randomness.
- \(P\): A large 256-bit prime modulus derived from the well-known Bitcoin secp256k1 curve to avoid predictable patterns that occur with power-of-two moduli.

### Constants Definition

- **Mixing Constant \(K\)** ("Golden ratio" based, 256-bit):
```
0x9e3779b97f4a7c159e3779b97f4a7c159e3779b97f4a7c159e3779b97f4a7c15
```

- **XOR Constant \(R\)** ("Randomized", 256-bit):
```
0xf39cc0605cedc835f39cc0605cedc835f39cc0605cedc835f39cc0605cedc835
```

- **Prime Modulus \(P\)** (secp256k1 prime, 256-bit):
```
0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
```

Each constant is explicitly represented as a 256-bit integer (four 64-bit segments).

---

## 2. Rationale Behind Constant Choices

These constants were specifically selected based on well-established cryptographic and mathematical principles:

- **Mixing Constant \(K\)**: Extended from the 64-bit golden ratio constant commonly utilized for uniform bit mixing in hashing algorithms. This ensures comprehensive and unbiased dispersion of input bits.
- **XOR Constant \(R\)**: Applied post multiplication to introduce additional nonlinear characteristics and bit scrambling, significantly improving diffusion.
- **Prime Modulus \(P\)**: The Bitcoin secp256k1 prime modulus was deliberately chosen for its cryptographic properties, including robustness and absence of small prime factors, thus mitigating bias and increasing collision resistance.

These combined operations ensure an effective and statistically robust hash function suitable for educational and illustrative purposes.

> **Security Disclaimer**: Although mathematically sound and informative, Seytan256 is intended solely for educational purposes. Real-world cryptographic applications must use peer-reviewed algorithms like SHA-256 or SHA-3.

---

## 3. In-Depth Implementation Details (C)

The Seytan256 algorithm is implemented in C and carefully manages large integers using arrays of four 64-bit words to represent 256-bit numbers. It performs fundamental big-integer arithmetic operations to handle the large numbers involved in the hashing computation.

### Core Big-Integer Functions

#### Addition (`add_256`)
Handles carry propagation in addition of two 256-bit integers.

```c
static void add_256(uint64_t r[4], const uint64_t a[4]) {
  __uint128_t carry = 0; 
  for(int i = 0; i < 4; i++) {
    __uint128_t sum = (__uint128_t)r[i] + a[i] + carry;
    r[i] = (uint64_t)sum;
    carry = sum >> 64;
  }
}
```

#### Multiplication (`mul_256_64`)
Performs multiplication of a 256-bit integer by a 64-bit integer, propagating carries.

```c
static void mul_256_64(uint64_t r[4], const uint64_t a[4], uint64_t b) {
  __uint128_t carry = 0;
  for(int i = 0; i < 4; i++) {
    __uint128_t prod = (__uint128_t)a[i] * b + carry;
    r[i] = (uint64_t)prod;
    carry = prod >> 64;
  }
}
```

#### XOR Operation (`xor_256`)
Applies a bitwise XOR across two 256-bit values.

```c
static void xor_256(uint64_t r[4], const uint64_t x[4]) {
  for(int i = 0; i < 4; i++) r[i] ^= x[i];
}
```

#### Modulus (`mod_256`)
Reduces the hash modulo prime \(P\) using comparison and subtraction operations:

```c
static void mod_256(uint64_t x[4]) {
  while(cmp_256(x, P) >= 0) sub_256(x, P);
}
```

### Hash Calculation Flow

The hash computation proceeds in the following sequence:

1. Initialize a 256-bit accumulator (`sum`) to zero.
2. Iterate over each character `s[i]` of the input:
   - Compute \(\text{val}(s_i)\times(i+1)\).
   - Multiply this result by constant \(K\).
   - XOR the product with constant \(R\).
   - Add the XOR result to the accumulator (`sum`).
   - Reduce the accumulator modulo \(P\) after each addition.
3. After processing all input characters, output the final 256-bit hash in hexadecimal.

---

## Compilation and Usage

Compile the provided C code (`seytan256.c`) using `gcc`:

```bash
gcc seytan256.c -o seytan256
```

Execute the binary to hash an input string:

```bash
./seytan256 your_input_string
```

The result will be a 64-digit hexadecimal hash.

---
