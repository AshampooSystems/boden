package io.boden.android;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

class NativeRecycleAdapter extends RecyclerView.Adapter<NativeRecycleAdapter.NativeViewHolder>
{
    public static class NativeViewHolder extends RecyclerView.ViewHolder {
        private io.boden.android.NativeViewGroup _viewGroup;

        public NativeViewHolder(io.boden.android.NativeViewGroup vg) {
            super(vg);
            _viewGroup = vg;
        }
    }

    @NonNull
    @Override
    public NativeViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return null;
    }

    @Override
    public void onBindViewHolder(@NonNull NativeViewHolder holder, int position) {

    }

    @Override
    public int getItemCount() {
        return 0;
    }
};