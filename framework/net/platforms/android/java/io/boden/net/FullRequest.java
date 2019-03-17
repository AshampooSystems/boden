package io.boden.java;

import androidx.annotation.GuardedBy;
import androidx.annotation.Nullable;
import com.android.volley.NetworkResponse;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.toolbox.HttpHeaderParser;
import java.io.UnsupportedEncodingException;
import java.util.Map;

/** A canned request for retrieving the response body at a given URL as a String. */
public class FullRequest extends Request<FullData> {

    /** Lock to guard mListener as it is cleared on cancel() and read on delivery. */
    private final Object mLock = new Object();

    @Nullable
    @GuardedBy("mLock")
    private Listener<FullData> mListener;

    /**
     * Creates a new request with the given method.
     *
     * @param method the request {@link Method} to use
     * @param url URL to fetch the string at
     * @param listener Listener to receive the String response
     * @param errorListener Error listener, or null to ignore errors
     */
    public FullRequest(
            int method,
            String url,
            Listener<FullData> listener,
            @Nullable ErrorListener errorListener) {
        super(method, url, errorListener);
        mListener = listener;
    }

    /**
     * Creates a new GET request.
     *
     * @param url URL to fetch the string at
     * @param listener Listener to receive the String response
     * @param errorListener Error listener, or null to ignore errors
     */
    public FullRequest(
            String url, Listener<FullData> listener, @Nullable ErrorListener errorListener) {
        this(Method.GET, url, listener, errorListener);
    }

    @Override
    public void cancel() {
        super.cancel();
        synchronized (mLock) {
            mListener = null;
        }
    }

    @Override
    protected void deliverResponse(FullData response) {
        Response.Listener<FullData> listener;
        synchronized (mLock) {
            listener = mListener;
        }
        if (listener != null) {
            listener.onResponse(response);
        }
    }

    @Override
    @SuppressWarnings("DefaultCharset")
    protected Response<FullData> parseNetworkResponse(NetworkResponse response) {
        FullData result = new FullData();
        result.statusCode = response.statusCode;

        String parsed;
        try {
            parsed = new String(response.data, HttpHeaderParser.parseCharset(response.headers));
        } catch (UnsupportedEncodingException e) {
            // Since minSdkVersion = 8, we can't call
            // new String(response.data, Charset.defaultCharset())
            // So suppress the warning instead.
            parsed = new String(response.data);
        }

        result.data = parsed;
        result.headers = "";
        for (Map.Entry<String, String> entry : response.headers.entrySet()) {
            result.headers += entry.getKey() + ": " + entry.getValue() + "\n\n";
        }

        return Response.success(result, HttpHeaderParser.parseCacheHeaders(response));
    }
}
