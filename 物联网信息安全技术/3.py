import random
# 扩展欧几里得算法
def ext_gcd(a, b):
    if b == 0:
        return 1, 0, a
    else:
        x, y, gcd = ext_gcd(b, a % b)
        x, y = y, (x - (a // b) * y)
        return x, y, gcd
# 求逆算法
def mod_inverse(a, m):
    x, y, gcd = ext_gcd(a, m)
    if gcd != 1:
        return None 
    else:
        return x % m
# 随机素数生成相关函数
def is_prime(num):
    if num < 2:
        return False
    for i in range(2, int(num**0.5) + 1):
        if num % i == 0:
            return False
    return True
def generate_prime(min_val, max_val):
    while True:
        num = random.randint(min_val, max_val)
        if is_prime(num):
            return num
# 3. RSA 密钥生成、加密与解密
def generate_keys():
    """生成 RSA 公钥和私钥"""
    p = generate_prime(100, 500)
    q = generate_prime(100, 500)
    while p == q:
        q = generate_prime(100, 500)
    # 计算 n 和 φ(n)   
    n = p * q
    phi_n = (p - 1) * (q - 1)
    # 私有密钥 d 随机产生，要求与 φ(n) 互质
    d = random.randint(2, phi_n - 1)
    while True:
        _, _, gcd_val = ext_gcd(d, phi_n)
        if gcd_val == 1:
            break
        d = random.randint(2, phi_n - 1)    
    # 公开密钥 e 求 d 在模 φ(n) 下的乘法逆元
    e = mod_inverse(d, phi_n)
    return (e, n), (d, n)
# RSA 加密解密函数
def encrypt(plaintext, public_key):
    e, n = public_key
    cipher = [(ord(char) ** e) % n for char in plaintext]
    return cipher
# 解密函数
def decrypt(ciphertext, private_key):
    d, n = private_key
    plain = [chr((char ** d) % n) for char in ciphertext]
    return ''.join(plain)
# 4. 主程序交互流程
if __name__ == '__main__':
    print("=== RSA 加密解密演示系统 (自动模式) ===")
    # 1. 生成密钥
    print("\n[系统] 正在生成密钥对...")
    public_key, private_key = generate_keys()
    print(f"随机生成的私钥 (d, n): {private_key}")
    print(f"通过求逆算法计算的公钥 (e, n): {public_key}")
    # 2. 加密阶段 (保留键盘输入明文)
    print("\n--- 加密过程 ---")
    message = input("请输入需要加密的明文: ")
    encrypted_msg = encrypt(message, public_key)
    print(f"加密后的密文 (数组形式): \n{encrypted_msg}")
    # 3. 解密阶段 (自动调用密文和私钥)
    print("\n--- 解密过程 ---")
    print(f"自动调用刚才生成的密文 {encrypted_msg} 和私钥 {private_key} 进行解密")
    # 直接将 encrypted_msg 和 private_key 传入解密函数
    decrypted_msg = decrypt(encrypted_msg, private_key)
    print(f"\n解密算法输出的明文: {decrypted_msg}")