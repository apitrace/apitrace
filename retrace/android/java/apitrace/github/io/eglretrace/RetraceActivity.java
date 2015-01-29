package apitrace.github.io.eglretrace;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Build;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import java.io.FileDescriptor;
import java.io.IOException;

public class RetraceActivity extends Activity implements SurfaceHolder.Callback2{

    private SurfaceView m_surface;
    private Servers m_servers;
    private QtCreatorDebugger m_debugger = new QtCreatorDebugger("eglretrace");

    public void setSurfaceSize(final int width, final int height) {
        synchronized (this) {
            runOnUiThread( new Runnable() {
                @Override
                public void run() {
                    if (width < height)
                        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
                    else
                        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                    m_surface.getHolder().setFixedSize(width, height);
                }
            });
        }
    }

    public static native boolean setStdoutFileDescriptor(FileDescriptor fd);
    public static native boolean setStderrFileDescriptor(FileDescriptor fd);

    public static native void onActivityCreatedNative(RetraceActivity activity);
    public static native void onSurfaceCreatedNative(Surface surface);
    public static native void onSurfaceChangedNative(Surface surface, int format, int width, int height);
    public static native void onSurfaceRedrawNeededNative(Surface surface);
    public static native void onSurfaceDestroyedNative();
    public static native void onActivityDestroyedNative();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (Build.VERSION.SDK_INT > 13) {
            int systemVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            if (Build.VERSION.SDK_INT > 15) {
                systemVisibility |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_FULLSCREEN;
                if (Build.VERSION.SDK_INT > 18)
                    systemVisibility |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
            }
            getWindow().getDecorView().setSystemUiVisibility(systemVisibility);
        }

        if (savedInstanceState == null) {
            System.loadLibrary("gnustl_shared");
            System.loadLibrary("retrace");
            m_debugger.connect(this);
        }
        onActivityCreatedNative(this);
        try {
            m_servers = new Servers("apitrace.github.io.eglretrace.stdout", "apitrace.github.io.eglretrace.stderr");
        } catch (IOException e) {
            e.printStackTrace();
        }

        RelativeLayout layout = new RelativeLayout(this);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        m_surface =  new SurfaceView(this);
        m_surface.getHolder().addCallback(this);
        layout.addView(m_surface, params);
        setContentView(layout);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        onSurfaceCreatedNative(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        onSurfaceChangedNative(holder.getSurface(), format, width, height);
    }

    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        onSurfaceRedrawNeededNative(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        onSurfaceDestroyedNative();
    }

    @Override
    protected void onDestroy() {
        if (m_servers != null)
            m_servers.close();
        m_debugger.close();
        onActivityDestroyedNative();
        super.onDestroy();
        System.exit(1);
    }
}
