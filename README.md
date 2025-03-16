# Seytan256 Hash: Mathematical Description and Technical Explanation

This document provides:

1. A **mathematical overview** of the 256-bit hash function.
2. A **rationale** (“proof sketch”) for why these parameters are chosen.
3. A **detailed look** at the C implementation, focusing on **how** it works.

---

## Mathematical Function

We define a **256-bit** hash function \( H(S) \) for a string \( S \). Let \( S \) have characters \( s_0, s_1, \dots, s_{n-1} \). Then:

\[
H(S) = \left(\sum_{i=0}^{n-1} \bigl(\text{val}(s_i)\times(i+1)\times K \oplus R\bigr)\right)\mod P
\]

Where:
- \(\text{val}(s_i)\) is the ASCII value of the character \( s_i \).
- \((i+1)\) ensures positions affect the result differently (simple order dependence).
- \(K\) is a 256-bit **“golden-ratio-like”** constant used to mix bits.
- \(R\) is another 256-bit constant, used to **XOR** every term for extra diffusion.
- \(P\) is a **large 256-bit prime**. We use the well-known **secp256k1** prime:
  \[P = 2^{256} - 2^{32} - 977.\]

### Constants

- **\(K\)** (256-bit), repeated “golden ratio”:
  ```
  0x9e3779b97f4a7c159e3779b97f4a7c159e3779b97f4a7c159e3779b97f4a7c15
  ```
- **\(R\)** (256-bit), repeated “random-ish” constant:
  ```
  0xf39cc0605cedc835f39cc0605cedc835f39cc0605cedc835f39cc0605cedc835
  ```
- **\(P\)** (256-bit prime):
  ```
  0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
  ```

Each of these fits in **256 bits** and is stored as four 64-bit words in the source code.

---

## Rationale

The choice of \(K\), \(R\), and \(P\) is guided by established cryptographic principles:

- **\(K\)**: The extended golden-ratio constant ensures uniform bit-mixing.
- **\(R\)**: Additional XOR scrambling provides extra diffusion.
- **\(P\)**: Using a prime modulus close to \(2^{256}\) eliminates potential biases from power-of-two moduli.

These choices collectively help disperse input bits and reduce collisions.

> **Note:** This hash function is illustrative. For real-world security, use vetted cryptographic hashes like SHA-256 or SHA-3.

---

## Implementation Details

The provided C implementation:
- Stores 256-bit values as arrays of four 64-bit words.
- Includes big-integer arithmetic operations: addition, subtraction, multiplication, and modulus.

### Core Operations

- **Addition (`add_256`)**: Adds two 256-bit numbers with carry.
- **Comparison (`cmp_256`)**: Compares two 256-bit numbers.
- **Subtraction (`sub_256`)**: Subtracts two 256-bit numbers with borrow.
- **Modulus (`mod_256`)**: Reduces modulo \(P\).
- **Multiplication (`mul_256_64`, `mul_256_256`)**: Multiplies 256-bit numbers by 64-bit and 256-bit numbers.
- **XOR (`xor_256`)**: Performs bitwise XOR on 256-bit numbers.
- **Hex Printing (`print_hex_256`)**: Outputs 256-bit numbers in hex.

### Main Hashing Procedure

1. Initialize `sum` to zero.
2. For each character `s[i]`:
   - Multiply ASCII value by position index.
   - Multiply by \(K\), XOR with \(R\), and add to `sum`.
   - Apply modulus \(P\).
3. Output the final `sum` as hex.

---

## Compilation & Usage

Compile with:
```bash
gcc seytan256.c -o seytan256
```

Run with:
```bash
./seytan256 your_input_string
```

The output is a 64-digit hexadecimal hash.

---

## Conclusion

This hash demonstrates foundational cryptographic techniques in big-integer arithmetic and modulus operations but isn't suited for production cryptographic use.
