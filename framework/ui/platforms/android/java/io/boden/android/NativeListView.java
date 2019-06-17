package io.boden.android;

import android.content.Context;
import androidx.annotation.NonNull;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import android.widget.ListView;

public class NativeListView extends SwipeRefreshLayout {
    public NativeListView(@NonNull Context context) {
        super(context);

        _listView = new ListView(getContext());
        addView(_listView);

        _listView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        setEnabled(true);

        setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                doRefresh();
            }
        });
    }

    ListView getListView() {
        return _listView;
    }

    private ListView _listView;

    private native void doRefresh();
}
