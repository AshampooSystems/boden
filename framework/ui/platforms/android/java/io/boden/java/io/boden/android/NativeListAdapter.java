package io.boden.android;

import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.ListView;
import android.view.View;
import android.view.ViewGroup;

public class NativeListAdapter extends BaseAdapter
{
    public NativeListAdapter(View view) {
        _view = view;
    }

    @Override
    public int getCount() {
        return nativeGetCount(_view);
    }

    // Everytime you call either getItemId() or getItem() a little bunny
    // will die in unprecedented pain.

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public Object getItem(int position) {
        return null;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup container) {
        if (convertView == null) {
            convertView = new TextView(container.getContext());
        }

        ((TextView)convertView).setText(nativeLabelTextForRowIndex(_view, position));

        return convertView;
    }

    public native int nativeGetCount(View view);
    public native String nativeLabelTextForRowIndex(View view, int rowIndex);

    private View _view;
}
