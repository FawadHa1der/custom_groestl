# Implementing Galois Field (2^8) multiplication for 203 * 7

def gf_multiply(a, b):
    """Multiply two numbers in the GF(2^8) finite field."""
    print("First number in binary:", format(a, '08b'))
    print("Second number in binary:", format(b, '08b'))
    product = 0
    for i in range(8):
        if b & 1:
            print("before adding xor product: :", product ,format(product, '08b'), "a:", a, format(a, '08b'))
            product ^= a
            print("product after xor product", product , format(product, '08b'))

        carry = a & 0x80
        print("left shift:" )
        a <<= 1
        if carry:
            a ^= 0x11b
            print("carry mod")
        b >>= 1
    # product ^= 0x11b
    return product

# Multiply 203 by 7
result = gf_multiply(4, 5)
print("The result of the multiplication is:", result, "in binary:", format(result, '08b'))
result
