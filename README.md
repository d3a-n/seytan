```markdown
# Seytan256 Hash: Mathematical Description and Technical Explanation

This document provides:

1. A **mathematical overview** of the 256-bit hash function.  
2. A **rationale** (“proof sketch”) for why these parameters are chosen.  
3. A **detailed look** at the C implementation, focusing on **how** it works.

---

## 1. Mathematical Function

We define a **256-bit** hash function \( H(S) \) for a string \( S \). Let \( S \) have characters \( s_0, s_1, \dots, s_{n-1} \). Then:

\[
\displaystyle
H(S) \;=\; \Bigl(\sum_{i=0}^{n-1} \bigl(\text{val}(s_i)\times(i+1)\times K \;\oplus\; R\bigr)\Bigr)\;\bmod\; P
\]

Where:
1. \(\text{val}(s_i)\) is the ASCII value of the character \( s_i \).  
2. \( (i+1) \) ensures positions affect the result differently (simple order dependence).  
3. \( K \) is a 256-bit **“golden-ratio-like”** constant used to mix bits.  
4. \( R \) is another 256-bit constant, used to **XOR** every term for extra diffusion.  
5. \( P \) is a **large 256-bit prime**. We use the well-known **secp256k1** prime:
   \[
   P = 2^{256} - 2^{32} - 977.
   \]

### 1.1 Constants

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

Each of these fits in **256 bits**. We store them as four 64-bit words in the source code.

---

## 2. Why These Choices? (Proof Sketch / Rationale)

While not a formal cryptographic proof, the selection of \( K \), \( R \), and \( P \) is guided by **well-known principles**:

1. **\(K\)**: The 64-bit constant `0x9e3779b97f4a7c15` is widely used for bit-mixing (often called a “golden ratio” constant). Extending it by concatenation to 256 bits produces a uniform, high-entropy constant for multiplication.  
2. **\(R\)**: A second constant ensures an **XOR scramble** after multiplication by \( K \). This injects an extra layer of non-linearity.  
3. **\(P\)**: By taking the result \(\bmod\; P\) with a **large prime** near \(2^{256}\), we avoid the biases that come from working in a power-of-two modulus. Using the **Bitcoin secp256k1** prime is a standard practice in elliptic curve cryptography, known to be robust and free of small prime factors.  

Collectively, these steps help **disperse** input bits and **reduce collisions**. The operations (\( \times \), \( \oplus \), \(\bmod P\)) cover linear and non-linear transformations that are common in hash functions.

> **Note**: For a production-grade cryptographic hash, a **fully vetted** design (e.g., SHA-256, SHA-3) is recommended. The approach here is mainly illustrative.

---

## 3. Implementation Details

Below is the **C implementation** of this 256-bit hash. It:

- Maintains 256-bit values as **four 64-bit words** in an array.  
- Implements **basic big-integer** addition, subtraction, multiplication, and modulo operations.  
- Uses the described formula directly on the characters of the input string.

### 3.1 Core Big-Integer Operations

**1. Addition (add_256)**  
Adds two 256-bit numbers (\( a, b \)) with carry handling:
```c
static void add_256(uint64_t r[4], const uint64_t a[4]) {
  __uint128_t c = 0; 
  for(int i=0; i<4; i++) {
    __uint128_t s = ( (__uint128_t)r[i] + a[i] + c );
    r[i] = (uint64_t)s;
    c = s >> 64;
  }
}
```

**2. Compare (cmp_256)**  
Returns -1, 0, or 1 depending on whether \( a < b \), \( a = b \), or \( a > b \):
```c
static int cmp_256(const uint64_t a[4], const uint64_t b[4]) {
  for(int i=3; i>=0; i--) {
    if(a[i] < b[i]) return -1;
    if(a[i] > b[i]) return 1;
  }
  return 0;
}
```

**3. Subtraction (sub_256)**  
Subtracts \((r - m)\) with borrow:
```c
static void sub_256(uint64_t r[4], const uint64_t m[4]) {
  __uint128_t c = 0;
  for(int i=0; i<4; i++) {
    __uint128_t diff = ( (__uint128_t)r[i] - m[i] - c );
    r[i] = (uint64_t)diff;
    c = (diff >> 64) & 1;
  }
}
```

**4. Modulus (mod_256)**  
Reduces \( x \mod P \) by repeatedly subtracting \( P \) if \( x \ge P \):
```c
static void mod_256(uint64_t x[4]) {
  while(cmp_256(x, P) >= 0) {
    sub_256(x, P);
  }
}
```

**5. Multiply a 256-bit by 64-bit (mul_256_64)**  
```c
static void mul_256_64(uint64_t r[4], const uint64_t a[4], uint64_t b) {
  __uint128_t c = 0;
  for(int i=0; i<4; i++) {
    __uint128_t m = ( (__uint128_t)a[i] * b ) + c;
    r[i] = (uint64_t)m;
    c = m >> 64;
  }
}
```

**6. Multiply a 256-bit by 256-bit (mul_256_256)**  
Uses repeated shifts and adds:
```c
static void mul_256_256(uint64_t r[4], const uint64_t a[4], const uint64_t b[4]) {
  uint64_t tmp[4] = {0}, res[4] = {0};
  for(int i=0; i<4; i++) {
    memset(tmp, 0, sizeof(tmp));
    mul_256_64(tmp, a, b[i]);
    for(int j=0; j<i; j++) {
      // shift left by 64 bits * i
      for(int k=3; k>0; k--) tmp[k] = tmp[k-1];
      tmp[0] = 0;
    }
    add_256(res, tmp);
  }
  memcpy(r, res, sizeof(res));
}
```

**7. XOR (xor_256)**  
Bitwise XOR of two 256-bit arrays:
```c
static void xor_256(uint64_t r[4], const uint64_t x[4]) {
  for(int i=0; i<4; i++) {
    r[i] ^= x[i];
  }
}
```

**8. Hex Printing (print_hex_256)**  
Prints the 256-bit result in big-endian hex (most significant word first):
```c
static void print_hex_256(const uint64_t x[4]) {
  for(int i=3; i>=0; i--) printf("%016lx", x[i]);
  printf("\n");
}
```

### 3.2 Main Hashing Procedure

1. **Initialize** `sum` to 0 (a 256-bit number).
2. **For each character** `s[i]`, do:
   - Compute \(\text{val}(s_i)\times (i+1)\).  
   - Multiply by \(K\) (256-bit multiplication by a 64-bit intermediate).  
   - **XOR** the result with \(R\).  
   - **Add** it to `sum`.  
   - **mod P** so `sum` remains within the prime field.  
3. At the end, print `sum` in 256-bit hex.

---

### 4 Compilation & Usage

```bash
# Compile:
cc seytan256.c -o seytan256

# Run:
./seytan256 hello
```

Output is a **64-hex-digit** number representing the hash.

---

## 5. Conclusion

- **Mathematical Approach**: Combines integer multiplication, XOR, and a **prime modulus** to produce a 256-bit result.  
- **Implementation**: Demonstrates big-integer arithmetic in C (managing 256-bit operations via 64-bit arrays).  
- **Security Note**: While it uses patterns from cryptographic primitives, this code **is not** a peer-reviewed cryptographic hash. For real security, use well-established functions (SHA-2, SHA-3, etc.).

```markdown
```