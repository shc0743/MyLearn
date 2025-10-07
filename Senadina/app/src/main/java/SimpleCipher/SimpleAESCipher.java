package SimpleCipher;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.util.Base64;

public class SimpleAESCipher {

    // 使用CBC模式和PKCS5填充
    private static final String TRANSFORMATION = "AES/CBC/PKCS5Padding";
    private static final String ALGORITHM = "AES";
    private static final int IV_LENGTH = 16; // AES块大小是16字节

    /**
     * 生成随机IV（初始化向量）
     */
    private static byte[] generateIV() {
        byte[] iv = new byte[IV_LENGTH];
        new SecureRandom().nextBytes(iv);
        return iv;
    }

    /**
     * 从文本密钥生成AES密钥
     */
    private static SecretKeySpec generateKey(String keyText) {
        byte[] keyBytes = keyText.getBytes(StandardCharsets.UTF_8);
        byte[] finalKey = new byte[16]; // 128位AES

        for (int i = 0; i < finalKey.length; i++) {
            if (i < keyBytes.length) {
                finalKey[i] = keyBytes[i];
            } else {
                finalKey[i] = 0;
            }
        }

        return new SecretKeySpec(finalKey, ALGORITHM);
    }

    /**
     * 加密方法
     */
    public static String encrypt(String plainText, String key) throws NoSuchPaddingException, NoSuchAlgorithmException, IllegalBlockSizeException, BadPaddingException, InvalidAlgorithmParameterException, InvalidKeyException {
        SecretKeySpec secretKey = generateKey(key);
        byte[] iv = generateIV();

        Cipher cipher = Cipher.getInstance(TRANSFORMATION);
        cipher.init(Cipher.ENCRYPT_MODE, secretKey, new IvParameterSpec(iv));

        byte[] encryptedBytes = cipher.doFinal(plainText.getBytes(StandardCharsets.UTF_8));

        // 将IV和加密数据组合在一起：IV + 加密数据
        byte[] combined = new byte[iv.length + encryptedBytes.length];
        System.arraycopy(iv, 0, combined, 0, iv.length);
        System.arraycopy(encryptedBytes, 0, combined, iv.length, encryptedBytes.length);

        return Base64.getEncoder().encodeToString(combined);
    }

    /**
     * 解密方法
     */
    public static String decrypt(String encryptedText, String key) throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidAlgorithmParameterException, InvalidKeyException, IllegalBlockSizeException, BadPaddingException {
        SecretKeySpec secretKey = generateKey(key);
        byte[] combined = Base64.getDecoder().decode(encryptedText);

        // 分离IV和加密数据
        byte[] iv = new byte[IV_LENGTH];
        byte[] encryptedBytes = new byte[combined.length - IV_LENGTH];

        System.arraycopy(combined, 0, iv, 0, IV_LENGTH);
        System.arraycopy(combined, IV_LENGTH, encryptedBytes, 0, encryptedBytes.length);

        Cipher cipher = Cipher.getInstance(TRANSFORMATION);
        cipher.init(Cipher.DECRYPT_MODE, secretKey, new IvParameterSpec(iv));

        byte[] decryptedBytes = cipher.doFinal(encryptedBytes);
        return new String(decryptedBytes, StandardCharsets.UTF_8);
    }
}