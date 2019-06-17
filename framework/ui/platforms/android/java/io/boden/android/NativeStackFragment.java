package io.boden.android;

import android.os.Bundle;

import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class NativeStackFragment extends Fragment {
    public NativeStackFragment() {

    }

    public void setActualView(View actualView) {
        _actualView = actualView;
    }

    public View getActualView() {
        return _actualView;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return _actualView;
    }

    private View _actualView;
}
