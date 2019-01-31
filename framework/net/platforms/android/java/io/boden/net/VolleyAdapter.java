package io.boden.java;

import android.content.Context;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.Response.Listener;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.BasicNetwork;
import com.android.volley.toolbox.HurlStack;
import com.android.volley.toolbox.NoCache;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

public class VolleyAdapter {
    private RequestQueue queue;

    public VolleyAdapter() {
        BasicNetwork network = new BasicNetwork(new HurlStack());
        queue = new RequestQueue(new NoCache(), network);
        queue.start();
    }

    public void request(int requestMethod, String url, final NativeStrongPointer nativeResponse) {
        FullRequest request = new FullRequest(requestMethod, url,
                new Response.Listener<FullData>() {
                    @Override
                    public void onResponse(FullData response) {
                        handleResponse(nativeResponse, response.statusCode, response.data, response.headers);//, "Hallo Welt", "Nothing!");
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                handleResponse(nativeResponse, -1, error.getMessage(), null);

            }
        });
        queue.add(request);
    }

    private native static void handleResponse(NativeStrongPointer nativeResponse, int statusCode, String data, String headers);
}
