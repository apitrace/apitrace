package apitrace.github.io.eglretrace;

import android.net.LocalSocket;

import java.io.IOException;

public class Servers {
    private AbstractServer m_stdout, m_stderr;
    Servers(String stdoutServerName, String stderrServerName) throws IOException {
        m_stdout = new AbstractServer(stdoutServerName) {
            @Override
            protected void accepted(LocalSocket socket) {
                if (!RetraceActivity.setStdoutFileDescriptor(socket.getFileDescriptor())) {
                    try {
                        // close socket if we are busy
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        m_stdout.start();
        m_stderr = new AbstractServer(stderrServerName) {
            @Override
            protected void accepted(LocalSocket socket) {
                if (!RetraceActivity.setStderrFileDescriptor(socket.getFileDescriptor())) {
                    try {
                        // close socket if we are busy
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        m_stderr.start();
    }

    public void close() {
        try {
            m_stdout.close();
            m_stdout.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
        try {
            m_stderr.close();
            m_stderr.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
