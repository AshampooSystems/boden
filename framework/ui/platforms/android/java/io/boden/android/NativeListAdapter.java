package io.boden.android;

import android.content.Context;
import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.ListView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.view.View;
import android.view.ViewGroup;

import io.boden.java.NativeStrongPointer;

public class NativeListAdapter extends BaseAdapter
{
    public class RowContainer extends NativeViewGroup {
        public RowContainer(Context context) {
            super(context);
        }

        public NativeStrongPointer getBdnView() {
            return _bdnViewPtr;
        }

        public void setBdnView(NativeStrongPointer bdnViewPtr) {
            _bdnViewPtr = bdnViewPtr;
        }

        private NativeStrongPointer _bdnViewPtr;
    }

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
        return nativeViewForRowIndex(_view, position, convertView, container);
    }

    public native int nativeGetCount(View view);
    public native String nativeLabelTextForRowIndex(View view, int rowIndex);
    public native View nativeViewForRowIndex(View view, int rowIndex, View reusableView, ViewGroup container);

    private View _view;
}
