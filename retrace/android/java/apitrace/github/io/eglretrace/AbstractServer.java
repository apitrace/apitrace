package apitrace.github.io.eglretrace;

import android.net.LocalServerSocket;
import android.net.LocalSocket;

import java.io.IOException;

public abstract class AbstractServer extends Thread {
    public AbstractServer(String name) throws IOException {
        m_localServer = new LocalServerSocket(name);
    }

    private LocalServerSocket m_localServer;

    public void close() throws IOException {
        m_localServer.close();
    }

    @Override
    public void run() {
        while(true) {
            try {
                accepted(m_localServer.accept());
            } catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }
    }
    protected abstract void accepted(LocalSocket socket);
}
