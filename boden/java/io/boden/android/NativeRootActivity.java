package io.boden.android;

import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;

/** An activity that is controlled by native code.
 *
 *  You can directly specify this activity in your AndroidManifest.xml.
 *
 *  The activity will load the native code library and initialize everything to allow
 *  the native code size to control the contents and behaviour of the activity.
 *
 *  In your manifest you can specify a meta value called "android.app.lib_name"
 *  to control the name of the native library that the activity will load. If no lib_name
 *  is given then the activity will try to load a lib called "main".
 *
 *
 * */
public class NativeRootActivity extends android.app.Activity
{
    /** Name of the meta variable that can be used in the manifest to specify the
     *  name of the native code library.
     *
     *  If this is not specified then "main" is used.
     */
    public static final String META_DATA_LIB_NAME = "io.boden.android.lib_name";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        loadMetaData();

        _libName = getMetaString(META_DATA_LIB_NAME, "main");

        NativeInit.baseInit(_libName);

        _rootView = new NativeRootView(this);
        setContentView( _rootView );

        NativeInit.launch();
    }

    @Override
    public void onConfigurationChanged (Configuration newConfig)
    {
        _rootView.onConfigurationChanged(newConfig);

        super.onConfigurationChanged(newConfig);
    }



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
        Object val = _metaData.get( META_DATA_LIB_NAME );
        if( val!=null && val instanceof String)
            return (String)val;
        else
            return defaultValue;
    }



    private NativeRootView _rootView;

    private Bundle _metaData;
    private String _libName;
}





