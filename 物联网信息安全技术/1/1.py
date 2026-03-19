def caesar_cipher(text, shift, mode='encrypt'):
    """
    单表代替密码：凯撒密码 (Caesar Cipher)
    通过将字母表中的每个字母移动固定的位置来进行加密或解密。
    """
    result = ""
    # 如果是解密模式，将位移量反转
    if mode == 'decrypt':
        shift = -shift
        
    for char in text:
        if char.isalpha():
            # 判断大小写以确定 ASCII 偏移量 (A=65, a=97)
            ascii_offset = 65 if char.isupper() else 97
            # 核心算法：(当前字母ASCII - 偏移量 + 位移) 取模 26，再加回偏移量
            result += chr((ord(char) - ascii_offset + shift) % 26 + ascii_offset)
        else:
            # 非字母字符保持不变
            result += char
            
    return result

def vigenere_cipher(text, key, mode='encrypt'):
    """
    多表代替密码：维吉尼亚密码 (Vigenère Cipher)
    使用一个关键词作为密钥，关键词的不同字母决定了明文中对应字母的偏移量。
    """
    result = ""
    key = key.upper()
    key_idx = 0
    
    for char in text:
        if char.isalpha():
            ascii_offset = 65 if char.isupper() else 97
            # 根据密钥当前字符计算偏移量 (A=0, B=1, ..., Z=25)
            shift = ord(key[key_idx % len(key)]) - 65
            
            if mode == 'decrypt':
                shift = -shift
                
            result += chr((ord(char) - ascii_offset + shift) % 26 + ascii_offset)
            key_idx += 1  # 只有在处理字母时，密钥索引才向后移动
        else:
            result += char
            
    return result

def brute_force_caesar(ciphertext):
    print(f"--- 穷举搜索法破译开始 ---")
    print(f"待破译密文: {ciphertext}\n")
    # 遍历所有可能的位移量 (1 到 25)
    for shift in range(1, 26):
        # 尝试使用当前位移量进行解密
        decrypted_text = caesar_cipher(ciphertext, shift, mode='decrypt')
        # 标记出每种偏移量的解密结果
        print(f"尝试密钥 (位移={shift:2d}) : {decrypted_text} ")
# ================= 测试代码 =================
if __name__ == "__main__":
    print("==================================================")
    print("1. 单表与多表代替密码演示")
    print("==================================================")
    plaintext = "Hello World!"
    
    print("\n[单表代替：凯撒密码 (位移量: 3)]")
    caesar_encrypted = caesar_cipher(plaintext, 3, mode='encrypt')
    caesar_decrypted = caesar_cipher(caesar_encrypted, 3, mode='decrypt')
    print(f"明文: {plaintext}")
    print(f"密文: {caesar_encrypted}")
    print(f"解密: {caesar_decrypted}")

    print("\n[多表代替：维吉尼亚密码 (密钥: KEY)]")
    vigenere_encrypted = vigenere_cipher(plaintext, "KEY", mode='encrypt')
    vigenere_decrypted = vigenere_cipher(vigenere_encrypted, "KEY", mode='decrypt')
    print(f"明文: {plaintext}")
    print(f"密文: {vigenere_encrypted}")
    print(f"解密: {vigenere_decrypted}")

    print("\n==================================================")
    print("2. 穷举搜索法破译密文")
    print("==================================================")
    
    # 需要破译的密文
    target_ciphertext = "BEEAKFYDJXUQYHYJIQRYHTYJIQFBQDUYJIIKFUHCQD"
    
    # 执行穷举破译
    brute_force_caesar(target_ciphertext)