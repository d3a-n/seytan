#include <stdio.h>
#include <stdint.h>
#include <string.h>

static const uint64_t K[4] = {
  0x7f4a7c159e3779b9ULL, 0x7f4a7c159e3779b9ULL,
  0x7f4a7c159e3779b9ULL, 0x7f4a7c159e3779b9ULL
};

static const uint64_t R[4] = {
  0x5cedc835f39cc060ULL, 0x5cedc835f39cc060ULL,
  0x5cedc835f39cc060ULL, 0x5cedc835f39cc060ULL
};

static const uint64_t P[4] = {
  0xfffffc2fffffffffULL, 0xfffffffffffffffeULL,
  0xffffffffffffffffULL, 0xffffffffffffffffULL
};

static void add_256(uint64_t r[4], const uint64_t a[4]) {
  __uint128_t c=0; 
  for(int i=0;i<4;i++){
    __uint128_t s=(__uint128_t)r[i]+a[i]+c; 
    r[i]=(uint64_t)s; 
    c=s>>64;
  }
}

static int cmp_256(const uint64_t a[4],const uint64_t b[4]){
  for(int i=3;i>=0;i--){
    if(a[i]<b[i])return -1; 
    if(a[i]>b[i])return 1;
  }
  return 0;
}

static void sub_256(uint64_t r[4],const uint64_t m[4]){
  __uint128_t c=0; 
  for(int i=0;i<4;i++){
    __uint128_t diff=(__uint128_t)r[i]-(__uint128_t)m[i]-c; 
    r[i]=(uint64_t)diff; 
    c=(diff>>64)&1;
  }
}

static void mod_256(uint64_t x[4]){
  while(cmp_256(x,P)>=0) sub_256(x,P);
}

static void mul_256_64(uint64_t r[4], const uint64_t a[4], uint64_t b){
  __uint128_t c=0; 
  for(int i=0;i<4;i++){
    __uint128_t m=(__uint128_t)a[i]*b + c; 
    r[i]=(uint64_t)m; 
    c=m>>64;
  }
}

static void mul_256_256(uint64_t r[4], const uint64_t a[4], const uint64_t b[4]){
  uint64_t tmp[4]={0}, res[4]={0};
  for(int i=0;i<4;i++){
    memset(tmp,0,sizeof(tmp));
    mul_256_64(tmp,a,b[i]);
    for(int j=0;j<i;j++){
      for(int k=3;k>0;k--) tmp[k]=tmp[k-1];
      tmp[0]=0;
    }
    add_256(res,tmp);
  }
  memcpy(r,res,sizeof(res));
}

static void xor_256(uint64_t r[4], const uint64_t x[4]){
  for(int i=0;i<4;i++) r[i]^=x[i];
}

static void print_hex_256(const uint64_t x[4]){
  for(int i=3;i>=0;i--) printf("%016lx", x[i]);
  printf("\n");
}

int main(int argc,char**argv){
  if(argc<2)return 1;
  uint64_t sum[4]={0};
  const char* s=argv[1];
  for(size_t i=0;i<strlen(s);i++){
    uint64_t tmp1[4]={0}, tmp2[4]={0}, term[4]={0};
    uint64_t c=(uint64_t)(unsigned char)s[i]*(i+1);
    mul_256_64(tmp1,K,c);
    mod_256(tmp1);
    memcpy(term,tmp1,sizeof(term));
    xor_256(term,R);
    add_256(sum,term);
    mod_256(sum);
  }
  mod_256(sum);
  print_hex_256(sum);
  return 0;
}
