package io.boden.android;

import io.boden.android.NativeInit;
import io.boden.android.NativeRootView;

import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;

import android.util.Pair;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.webkit.MimeTypeMap;
import android.webkit.ValueCallback;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Collections;
import java.util.EventListener;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import static android.content.res.Configuration.UI_MODE_NIGHT_MASK;

public class NativeRootActivity extends AppCompatActivity
{
    public interface BackButtonListener extends EventListener {
        boolean backButtonPressed();
    }

    class BackButtonHandlers {
        final Vector<BackButtonListener> listener = new Vector<>();

        void add(BackButtonListener a) {
            listener.addElement(a);
        }

        void remove(BackButtonListener l) {
            listener.remove(l);
        }

        boolean handleBackButtonPressed() {
            for(int i=0; i < listener.size(); i++) {
                if ((listener.elementAt(i)).backButtonPressed()) {
                    return true;
                }
            }
            return false;
        }
    }

    class RedirectIntent extends Intent
    {
        public ValueCallback<Intent> mCallback;
        public RedirectIntent(String action) {
            super(action);
        }
    }

    private HashMap<Integer, ValueCallback<Pair<Intent, Integer>>> mActivityCallbacks = new HashMap<Integer, ValueCallback<Pair<Intent, Integer>>>();

    public Integer startActivityForResult(Intent intent, ValueCallback<Pair<Intent,Integer>> callback)
    {
        int m;
        if(mActivityCallbacks.size() == 0) {
            m = 0;
        } else {
            m = Collections.max(mActivityCallbacks.keySet()) + 1;
        }

        mActivityCallbacks.put(m, callback);

        startActivityForResult(intent, m);

        return m;
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        if(mActivityCallbacks.containsKey(requestCode)) {
            mActivityCallbacks.get(requestCode).onReceiveValue(new Pair<Intent, Integer>(intent, resultCode));
            mActivityCallbacks.remove(requestCode);
        }
    }

    /** Name of the meta variable that can be used in the manifest to specify the
     *  name of the native code library.
     *
     *  If this is not specified then "main" is used.
     */
    private static final String META_DATA_LIB_NAME = "io.boden.android.lib_name";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
        _rootActivity = this;
        _backButtonHandlers = new BackButtonHandlers();

        super.onCreate(savedInstanceState);

        loadMetaData();

        _libName = getMetaString(META_DATA_LIB_NAME, "main");

        NativeInit.baseInit(_libName);

        _rootView = new NativeRootView(this );
        setContentView(_rootView);

        nativeRegisterAppContext(this);


        _currentDayNightMode = getResources().getConfiguration().uiMode & UI_MODE_NIGHT_MASK;

        NativeInit.launch( getIntent() );
    }

    @Override
    public void onResume()
    {
        super.onResume();
    }

    @Override
    public void onDestroy()
    {
        NativeInit.onDestroy();
        super.onDestroy();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private static int getResourceIdFromString(String resName, String type) {
        try {
            String libName = NativeRootActivity.getRootActivity()._libName;
            Class<?> cls = Class.forName("io.boden.android." + libName + ".R$" + type);

            Field idField = cls.getDeclaredField(resName);
            return idField.getInt(idField);
        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }

    public static InputStream getStreamFromURI(String uri) {
        try {
            uri = NativeRootActivity.getResourceURIFromURI(uri);

            URI aUri = null;
            aUri = new URI(uri);

            String scheme = aUri.getScheme();
            String path = aUri.getPath();

            if (scheme.equals("file")) {
                InputStream stream = null;
                if (path.startsWith("/android_asset/")) {
                    stream = NativeRootActivity.getRootActivity().getApplicationContext().getAssets().open(path.substring(15));
                } else {
                    int id = NativeRootActivity.getResourceIdFromURI(uri);
                    if(id >= 0) {
                        stream = NativeRootActivity.getRootActivity().getApplicationContext().getResources().openRawResource(id);
                    }
                }

                if(stream != null)
                    return stream;
            }

            URL url = new URL(uri);
            return (InputStream) url.getContent();

        } catch (URISyntaxException e) {
            e.printStackTrace();
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }

    public static int getResourceIdFromURI(String uri) {
        try {
            URI aUri = new URI(uri);
            String host = aUri.getHost();
            String scheme = aUri.getScheme();
            String path = aUri.getPath();
            if(host != null && !host.isEmpty() && !host.equals("main")) {
                return -1; // Not supported yet.
            }

            if(scheme.equals("file")) {
                path = path.substring(1);
                String[] components = path.split("/");
                if(components.length != 3) {
                    return -1;
                }

                String root = components[0];

                if(!root.equals("android_res")) {
                    return -1;
                }

                String type = components[1];
                String name = components[2];

                name = name.substring(0,name.lastIndexOf('.'));

                return getResourceIdFromString(name, type);
            }


        } catch(URISyntaxException e) {
            e.printStackTrace();
        }

        return -1;
    }

    public static final String getResourceURIFromURI(String uri) {
        try {
            URI aUri = new URI(uri);
            String host = aUri.getHost();
            String scheme = aUri.getScheme();
            if(!host.isEmpty() && !host.equals("main")) {
                return uri; // Not supported yet.
            }

            String path = aUri.getPath();

            if(scheme.equals("asset")) {
                return "file:///android_asset" + path;
            }

            path = path.substring(1); // Remove leading /
            path = path.replace('/', '_');
            String extension = MimeTypeMap.getFileExtensionFromUrl(path);
            path = path.replace('.', '_');
            path = path.replace('-', '_');
            path += '.' + extension;

            if(scheme.equals("image")) {
                return "file:///android_res/drawable/" + path;
            }
            if(scheme.equals("resource")) {
                return "file:///android_res/raw/" + path;
            }

        } catch(URISyntaxException e) {
            return "";
        }

        return uri;
    }

    public static final void copyToClipboard(String str)
    {
        ClipboardManager clipboard = (ClipboardManager)getRootActivity().getSystemService(Context.CLIPBOARD_SERVICE);
        clipboard.setText(str);
    }

    @Override
    public void onConfigurationChanged (Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        _rootView.onConfigurationChanged(newConfig);

        int newDayNightMode = newConfig.uiMode & UI_MODE_NIGHT_MASK;

        if(_currentDayNightMode != newDayNightMode) {
            _currentDayNightMode = newDayNightMode;
            this.recreate();
        }
    }

    @Override
    public void onBackPressed() {
        if(!_backButtonHandlers.handleBackButtonPressed()) {
            super.onBackPressed();
        }
    }

    public void addBackButtonListener(BackButtonListener listener) {
        _backButtonHandlers.add(listener);
    }

    public void removeBackButtonListener(BackButtonListener listener) {
        _backButtonHandlers.remove(listener);
    }

    public static NativeRootActivity getRootActivity() {
        return _rootActivity;
    }
    public static NativeRootView getRootView() { return getRootActivity()._rootView; }

    private void loadMetaData()
    {
        try
        {
            ActivityInfo activityInfo = getPackageManager().getActivityInfo(
                    getComponentName(), PackageManager.GET_ACTIVITIES | PackageManager.GET_META_DATA);

            _metaData = activityInfo.metaData;
        }
        catch(PackageManager.NameNotFoundException e)
        {
            // activity is not in the manifest. Treat this like empty meta data
            _metaData = null;
        }

        if(_metaData==null)
            _metaData = new Bundle();
    }

    private String getMetaString(String valueName, String defaultValue)
    {
        Object val = _metaData.get( valueName );
        if( val instanceof String)
            return (String)val;
        else
            return defaultValue;
    }

    private static native void nativeRegisterAppContext(Context ctxt);

    public static native void nativeViewNeedsLayout(View view);

    private NativeRootView _rootView;
    private Bundle _metaData;
    private String _libName;
    private static NativeRootActivity _rootActivity;
    private int _currentDayNightMode;
    private BackButtonHandlers _backButtonHandlers;
}





