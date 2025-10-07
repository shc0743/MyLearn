import SimpleCipher.SimpleAESCipher;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

public class SenadinaIsUnique {
    
    // 用于存储资源访问的上下文
    private static Object context;
    
    public static void setContext(Object ctx) {
        context = ctx;
    }
    
    // 通过反射获取资源
    private static InputStream getRawResource(int resId) throws Exception {
        Class<?> resourcesClass = Class.forName("android.content.res.Resources");
        Class<?> contextClass = Class.forName("android.content.Context");
        
        java.lang.reflect.Method getResourcesMethod = contextClass.getMethod("getResources");
        Object resources = getResourcesMethod.invoke(context);
        
        java.lang.reflect.Method openRawResourceMethod = resourcesClass.getMethod("openRawResource", int.class);
        return (InputStream) openRawResourceMethod.invoke(resources, resId);
    }
    
    private static String getString(int resId) throws Exception {
        Class<?> resourcesClass = Class.forName("android.content.res.Resources");
        Class<?> contextClass = Class.forName("android.content.Context");
        
        java.lang.reflect.Method getResourcesMethod = contextClass.getMethod("getResources");
        Object resources = getResourcesMethod.invoke(context);
        
        java.lang.reflect.Method getStringMethod = resourcesClass.getMethod("getString", int.class);
        return (String) getStringMethod.invoke(resources, resId);
    }
    
    public static void main(String[] args) {
        try {
            // 通过反射获取 R.raw.senadina 的值
            Class<?> rRawClass = Class.forName("Senadina.Senadina$R$raw");
            java.lang.reflect.Field senadinaField = rRawClass.getField("senadina");
            int senadinaResId = (int) senadinaField.get(null);
            
            // 通过反射获取 R.string.senadina_encrypt_key_encrypted 的值
            Class<?> rStringClass = Class.forName("Senadina.Senadina$R$string");
            java.lang.reflect.Field keyField = rStringClass.getField("senadina_encrypt_key_encrypted");
            int keyResId = (int) keyField.get(null);
            
            // 获取资源
            InputStream inputStream = getRawResource(senadinaResId);
            byte[] buffer = new byte[inputStream.available()];
            inputStream.read(buffer);
            inputStream.close();
            
            String encrypted = new String(buffer, StandardCharsets.UTF_8);
            String encryptedKey = getString(keyResId);
            
            String decryptedKey = SimpleAESCipher.decrypt(encryptedKey, "希娜狄雅不一样！！！");
            String decrypted = SimpleAESCipher.decrypt(encrypted, decryptedKey);
            
            System.out.println(decrypted);
            
        } catch (Exception e) {
            System.out.println("Failed to load data: " + e.getMessage());
            e.printStackTrace();
        }
    }
}