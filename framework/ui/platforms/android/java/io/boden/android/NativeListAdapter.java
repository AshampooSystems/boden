package io.boden.android;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class NativeListAdapter extends RecyclerView.Adapter<NativeListAdapter.NativeViewHolder>
{
    public class ViewHolderViewGroup extends io.boden.android.NativeViewGroup
    {
        public ViewHolderViewGroup(Context context) {
            super(context);
        }

        public ViewHolderViewGroup(Context context, AttributeSet attrs) {
            super(context, attrs);
        }

        public ViewHolderViewGroup(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
        }

        public RecyclerView.ViewHolder _viewHolder;
    }

    private static abstract class ClickHandler implements View.OnClickListener {
        public int _position;
        ClickHandler(int position) {
            _position = position;
        }
    }

    @NonNull
    @Override
    public NativeViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        ViewHolderViewGroup viewGroup = new ViewHolderViewGroup(parent.getContext());

        NativeViewHolder vh = new NativeViewHolder(viewGroup);
        viewGroup.setClickable(true);

        TypedValue outValue = new TypedValue();
        parent.getContext().getTheme().resolveAttribute(android.R.attr.selectableItemBackground, outValue, true);
        viewGroup.setBackgroundResource(outValue.resourceId);

        return vh;
    }

    @Override
    public void onBindViewHolder(@NonNull NativeViewHolder holder, int position) {
        View newUserView = nativeViewForRowIndex(_view.getRecyclerView(), position, holder._userView, holder._viewGroup );

        holder._viewGroup.setOnClickListener(new ClickHandler(position) {
            @Override
            public void onClick(View v) {
                _view.nativeItemClicked(_position);
            }
        });

        if(holder._userView != newUserView) {
            if(holder._userView != null) {
                holder._viewGroup.removeAllViews();
            }

            holder._userView = newUserView;
            holder.position = position;

            if(holder._userView != null) {
                holder._viewGroup.addView(holder._userView);
                Log.d("boden", holder._userView.getMeasuredWidth() + "x" +  holder._userView.getMeasuredHeight());

                holder._viewGroup.setLayoutParams(new ViewGroup.LayoutParams(holder._userView.getMeasuredWidth(), holder._userView.getMeasuredHeight()));
                holder._viewGroup.setChildBounds(holder._userView, 0, 0, holder._userView.getMeasuredWidth(), holder._userView.getMeasuredHeight());
            }
        }
    }

    @Override
    public int getItemCount() {
        return nativeGetCount(_view.getRecyclerView());
    }

    public static class NativeViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public ViewHolderViewGroup _viewGroup;
        public View _userView = null;
        public int position;


        public NativeViewHolder(ViewHolderViewGroup viewGroup) {
            super(viewGroup);
            _viewGroup = viewGroup;
            _viewGroup._viewHolder = this;
        }
    }

    public NativeListAdapter(io.boden.android.NativeListView view) {
        _view = view;
    }

    public native int nativeGetCount(View view);
    public native View nativeViewForRowIndex(View view, int rowIndex, View reusableView, ViewGroup container);

    private io.boden.android.NativeListView _view;
}
