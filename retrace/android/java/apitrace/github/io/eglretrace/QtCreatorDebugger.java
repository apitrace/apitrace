package apitrace.github.io.eglretrace;

import android.app.Activity;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;

public class QtCreatorDebugger {

    QtCreatorDebugger(){}

    QtCreatorDebugger(String TAG) {
        this.TAG =TAG;
    }

    private String TAG = "QtCreator";

    private Process m_debuggerProcess = null; // debugger process

    public boolean connect(Activity activity) {
        try {
            Bundle extras = activity.getIntent().getExtras();

            if ( /*(ai.flags&ApplicationInfo.FLAG_DEBUGGABLE) != 0
                        &&*/ extras.containsKey("debug_ping")
                    && extras.getString("debug_ping").equals("true")) {
                try {
                    debugLog("extra parameters: " + extras);
                    String packageName = activity.getPackageName();
                    String pingFile = extras.getString("ping_file");
                    String pongFile = extras.getString("pong_file");
                    String gdbserverSocket = extras.getString("gdbserver_socket");
                    String gdbserverCommand = extras.getString("gdbserver_command");
                    String pingSocket = extras.getString("ping_socket");
                    boolean usePing = pingFile != null;
                    boolean usePong = pongFile != null;
                    boolean useSocket = gdbserverSocket != null;
                    boolean usePingSocket = pingSocket != null;
                    int napTime = 200; // milliseconds between file accesses
                    int timeOut = 30000; // ms until we give up on ping and pong
                    int maxAttempts = timeOut / napTime;

                    if (gdbserverSocket != null) {
                        debugLog("removing gdb socket " + gdbserverSocket);
                        new File(gdbserverSocket).delete();
                    }

                    if (usePing) {
                        debugLog("removing ping file " + pingFile);
                        File ping = new File(pingFile);
                        if (ping.exists()) {
                            if (!ping.delete())
                                debugLog("ping file cannot be deleted");
                        }
                    }

                    if (usePong) {
                        debugLog("removing pong file " + pongFile);
                        File pong = new File(pongFile);
                        if (pong.exists()) {
                            if (!pong.delete())
                                debugLog("pong file cannot be deleted");
                        }
                    }

                    debugLog("starting " + gdbserverCommand);
                    m_debuggerProcess = Runtime.getRuntime().exec(gdbserverCommand);
                    debugLog("gdbserver started");

                    if (useSocket) {
                        int i;
                        for (i = 0; i < maxAttempts; ++i) {
                            debugLog("waiting for socket at " + gdbserverSocket + ", attempt " + i);
                            File file = new File(gdbserverSocket);
                            if (file.exists()) {
                                file.setReadable(true, false);
                                file.setWritable(true, false);
                                file.setExecutable(true, false);
                                break;
                            }
                            Thread.sleep(napTime);
                        }

                        if (i == maxAttempts) {
                            debugLog("time out when waiting for debug socket");
                            return false;
                        }

                        debugLog("socket ok");
                    } else {
                        debugLog("socket not used");
                    }

                    if (usePingSocket) {
                        DebugWaitRunnable runnable = new DebugWaitRunnable(pingSocket);
                        Thread waitThread = new Thread(runnable);
                        waitThread.start();

                        int i;
                        for (i = 0; i < maxAttempts && waitThread.isAlive(); ++i) {
                            debugLog("Waiting for debug socket connect");
                            debugLog("go to sleep");
                            Thread.sleep(napTime);
                        }

                        if (i == maxAttempts) {
                            debugLog("time out when waiting for ping socket");
                            runnable.shutdown();
                            return false;
                        }

                        if (runnable.wasFailure) {
                            debugLog("Could not connect to debug client");
                            return false;
                        } else {
                            debugLog("Got pid acknowledgment");
                        }
                    }

                    if (usePing) {
                        // Tell we are ready.
                        debugLog("writing ping at " + pingFile);
                        FileWriter writer = new FileWriter(pingFile);
                        writer.write("" + android.os.Process.myPid());
                        writer.close();
                        File file = new File(pingFile);
                        file.setReadable(true, false);
                        file.setWritable(true, false);
                        file.setExecutable(true, false);
                        debugLog("wrote ping");
                    } else {
                        debugLog("ping not requested");
                    }

                    // Wait until other side is ready.
                    if (usePong) {
                        int i;
                        for (i = 0; i < maxAttempts; ++i) {
                            debugLog("waiting for pong at " + pongFile + ", attempt " + i);
                            File file = new File(pongFile);
                            if (file.exists()) {
                                file.delete();
                                break;
                            }
                            debugLog("go to sleep");
                            Thread.sleep(napTime);
                        }
                        debugLog("Removing pingFile " + pingFile);
                        new File(pingFile).delete();

                        if (i == maxAttempts) {
                            debugLog("time out when waiting for pong file");
                            return false;
                        }

                        debugLog("got pong " + pongFile);
                    } else {
                        debugLog("pong not requested");
                    }

                } catch (Exception e) {
                    debugLog("Can't start debugger" + e.getMessage());
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    void close()
    {
        if (m_debuggerProcess != null)
            m_debuggerProcess.destroy();
    }

    private void debugLog(String msg)
    {
        Log.i(TAG, "DEBUGGER: " + msg);
    }

    private class DebugWaitRunnable implements Runnable {

        public DebugWaitRunnable(String pingPongSocket) throws IOException {
            socket = new LocalServerSocket(pingPongSocket);
        }

        public boolean wasFailure;
        private LocalServerSocket socket;

        public void run() {
            final int napTime = 200; // milliseconds between file accesses
            final int timeOut = 30000; // ms until we give up on ping and pong
            final int maxAttempts = timeOut / napTime;

            try {
                LocalSocket connectionFromClient = socket.accept();
                debugLog("Debug socket accepted");
                BufferedReader inFromClient =
                        new BufferedReader(new InputStreamReader(connectionFromClient.getInputStream()));
                DataOutputStream outToClient = new DataOutputStream(connectionFromClient.getOutputStream());
                outToClient.writeBytes("" + android.os.Process.myPid());

                for (int i = 0; i < maxAttempts; i++) {
                    String clientData = inFromClient.readLine();
                    debugLog("Incoming socket " + clientData);
                    if (!clientData.isEmpty())
                        break;

                    if (connectionFromClient.isClosed()) {
                        wasFailure = true;
                        break;
                    }
                    Thread.sleep(napTime);
                }
            } catch (IOException ioEx) {
                ioEx.printStackTrace();
                wasFailure = true;
                debugLog("Can't start debugger" + ioEx.getMessage());
            } catch (InterruptedException interruptEx) {
                wasFailure = true;
                debugLog("Can't start debugger" + interruptEx.getMessage());
            }
        }

        public void shutdown() throws IOException
        {
            wasFailure = true;
            try {
                socket.close();
            } catch (IOException ignored) { }
        }
    };
}
