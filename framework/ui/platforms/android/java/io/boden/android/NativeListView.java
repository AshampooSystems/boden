package io.boden.android;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.view.View;


import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

public class NativeListView extends SwipeRefreshLayout {

    public static class SwipeCallback extends ItemTouchHelper.SimpleCallback
    {
        private NativeListView _listView;
        private ColorDrawable background = new ColorDrawable();
        Drawable deleteIcon;

        private int deleteIconIntrinsicWidth;
        private int deleteIconIntrinsicHeight;
        private int deleteColor;

        public SwipeCallback(Context context, NativeListView listView, int dragDirs, int swipeDirs) {
            super(dragDirs, swipeDirs);
            _listView = listView;
            deleteIcon = ContextCompat.getDrawable(context, android.R.drawable.ic_input_delete);

            deleteIconIntrinsicWidth = deleteIcon.getIntrinsicWidth();
            deleteIconIntrinsicHeight = deleteIcon.getIntrinsicHeight();

            deleteColor = ContextCompat.getColor(context, android.R.color.holo_red_light);
            background.setColor(deleteColor);
        }

        @Override
        public boolean onMove(@NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder, @NonNull RecyclerView.ViewHolder target) {
            return false;
        }

        @Override
        public void onSwiped(@NonNull RecyclerView.ViewHolder viewHolder, int direction) {
            int pos = viewHolder.getAdapterPosition();
            _listView.nativeItemDelete(pos);
            _listView._adapter.notifyItemRemoved(pos);
        }

        @Override
        public void onChildDraw(@NonNull Canvas canvas, @NonNull RecyclerView recyclerView, @NonNull RecyclerView.ViewHolder viewHolder, float dX, float dY, int actionState, boolean isCurrentlyActive) {
            View itemView = viewHolder.itemView;
            int itemHeight = itemView.getHeight();
            boolean isCanceled = dX == 0f && !isCurrentlyActive;

            if (isCanceled) {
                Paint clearPaint = new Paint();
                clearPaint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));

                canvas.drawRect( itemView.getRight() + dX, itemView.getTop(), itemView.getRight(), itemView.getBottom(), clearPaint);
                super.onChildDraw(canvas, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
                return;
            }

            background.setBounds(
                    itemView.getRight() + ((int) dX),
                    itemView.getTop(),
                    itemView.getRight(),
                    itemView.getBottom()
            );

            background.draw(canvas);

            // Calculate position of delete icon
            int iconTop = itemView.getTop() + (itemHeight - deleteIconIntrinsicHeight) / 2;
            int iconMargin = (itemHeight - deleteIconIntrinsicHeight) / 2;
            int iconLeft = itemView.getRight() - iconMargin - deleteIconIntrinsicWidth;
            int iconRight = itemView.getRight() - iconMargin;
            int iconBottom = iconTop + deleteIconIntrinsicHeight;

            // Draw the delete icon
            deleteIcon.setBounds(iconLeft, iconTop, iconRight, iconBottom);
            deleteIcon.draw(canvas);

            super.onChildDraw(canvas, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
        }
    }

    public NativeListView(@NonNull Context context) {
        super(context);

        _recyclerView = new RecyclerView(getContext());
        _recyclerView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        LinearLayoutManager layoutManager = new LinearLayoutManager(this.getContext());
        _recyclerView.setLayoutManager(layoutManager);
        _recyclerView.addItemDecoration(new DividerItemDecoration(_recyclerView.getContext(), layoutManager.getOrientation()));

        addView(_recyclerView);

        _adapter = new io.boden.android.NativeListAdapter(this);
        _adapter.setHasStableIds(true);
        _recyclerView.setAdapter(_adapter);

        _touchHelper = new ItemTouchHelper(new SwipeCallback(context, this, 0, ItemTouchHelper.LEFT));

        setEnabled(false);

        setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                doRefresh();
            }
        });
    }

    public void setEnableSwipeToDelete(boolean enable) {

        if(enable) {
            _touchHelper.attachToRecyclerView(_recyclerView);
        } else {
            _touchHelper.attachToRecyclerView(null);
        }
    }

    public int getRowIndexForView(View v) {
        while(v != null) {
            if(v instanceof io.boden.android.NativeListAdapter.ViewHolderViewGroup) {
                break;
            }
            v = (View)v.getParent();
        }

        if(v != null) {
            int pos = ((io.boden.android.NativeListAdapter.ViewHolderViewGroup) v)._viewHolder.getAdapterPosition();
            return pos;
        }
        return -1;
    }

    public void reloadData() {
        _adapter.notifyDataSetChanged();
    }

    RecyclerView getRecyclerView() {
        return _recyclerView;
    }

    private RecyclerView _recyclerView;
    private io.boden.android.NativeListAdapter _adapter;
    private ItemTouchHelper _touchHelper;

    private native void doRefresh();
    public native void nativeItemClicked(int position);
    public native void nativeItemDelete(int position);
}
