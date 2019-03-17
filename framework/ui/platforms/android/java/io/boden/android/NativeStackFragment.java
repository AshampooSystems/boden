package io.boden.android;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class NativeStackFragment extends Fragment {
    public NativeStackFragment() {

    }

    public void setView(View view) {
        _view = view;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return _view;
    }

    private View _view;
}
