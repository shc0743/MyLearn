package Senadina.Senadina;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.webkit.WebResourceRequest;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

import Senadina.Senadina.R;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

import SimpleCipher.SimpleAESCipher;

public class Senadina extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (getActionBar() != null) {
            getActionBar().setDisplayHomeAsUpEnabled(true);
            getActionBar().setTitle(R.string.senadina);
        }

        WebView w = findViewById(R.id.webviewSenadina);
        if (w != null) {
            w.getSettings().setJavaScriptEnabled(true);
            w.getSettings().setDomStorageEnabled(true);
            w.getSettings().setAllowFileAccess(true);
            try {
                InputStream inputStream = getResources().openRawResource(R.raw.senadina);
                byte[] buffer = new byte[inputStream.available()];
                inputStream.read(buffer);
                inputStream.close();
                String encrypted = new String(buffer, StandardCharsets.UTF_8);
                String decrypted = SimpleAESCipher.decrypt(encrypted, SimpleAESCipher.decrypt(getString(R.string.senadina_encrypt_key_encrypted), "希娜狄雅不一样！！！"));
                String htmlContent = "<div style=\"white-space: pre-wrap;\">" + decrypted + "</div>";
                w.loadDataWithBaseURL("file:///android_res/raw/", htmlContent, "text/html", "UTF-8", null);
            } catch (Exception e) {
                w.loadData("加载失败...", "text/html", "UTF-8");
            }
        }
    }

    @Override
    public boolean onNavigateUp() {
        finish();
        return true;
    }
}