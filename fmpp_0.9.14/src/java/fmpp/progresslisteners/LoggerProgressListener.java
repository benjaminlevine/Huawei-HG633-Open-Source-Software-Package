package fmpp.progresslisteners;

/*
 * Copyright (c) 2003, D�niel D�k�ny
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name "FMPP" nor the names of the project contributors may
 *   be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.Reader;
import java.util.Date;

import fmpp.Engine;
import fmpp.ProcessingException;
import fmpp.ProgressListener;
import fmpp.util.FileUtil;
import fmpp.util.MiscUtil;
import fmpp.util.StringUtil;


/**
 * Writes detailed information about errors and warning messages into a file
 * or into any character stream.
 */
public class LoggerProgressListener implements ProgressListener {
    private static final String LOG_FILE_HEADER = "FMPP log file";
    private static final String LOG_FILE_HEADER_OLD = "--- BEGIN LOG FILE ---";
    private final PrintWriter out;

    private int executedCount;
    private int copiedCount;
    private int renderedCount;
    private int errorCount;
    private int warningCount;
    private long startTime;

    /**
     * Output will be printed to the stderr.
     */
    public LoggerProgressListener() {
        this(System.err);
    }

    public LoggerProgressListener(OutputStream out) {
        this(new PrintWriter(out, true));
    }

    public LoggerProgressListener(PrintWriter out) {
        this.out = out;
    }

    public LoggerProgressListener(File logFile) throws IOException {
        this(logFile, false);
    }
    
    public LoggerProgressListener(File logFile, boolean append)
            throws IOException {
        // check what do we prepare to overwrite
        boolean logFileExists = logFile.exists() && logFile.length() != 0; 
        if (logFileExists) {
            Reader r = new FileReader(logFile);
            try {
                int c;
                boolean h1 = true;
                boolean h2 = true;
                int ln1 = LOG_FILE_HEADER.length();
                int ln2 = LOG_FILE_HEADER_OLD.length();
                int i = 0;
                while (h1 || h2) {
                    c = r.read();
                    if (h1) {
                        if (LOG_FILE_HEADER.charAt(i) != c) {
                            h1 = false;
                        }
                        if (i == ln1 - 1) {
                            break;
                        }
                    }
                    if (h2) {
                        if (LOG_FILE_HEADER_OLD.charAt(i) != c) {
                            h2 = false;
                        }
                        if (i == ln2 - 1) {
                            break;
                        }
                    }
                    i++;
                }
                if (!h1 && !h2) {
                    throw new IOException(
                            "Do not want to modify the log file, because "
                            + "it seems not be an FMPP log file. "
                            + "If this is a corruped log file, please delete "
                            + "it manually.");
                }
            } finally {
                r.close();
            }
        }

        out = new PrintWriter(new FileWriter(
                logFile.getAbsolutePath(), append), true);
        if (!append || !logFileExists) {
            out.println(LOG_FILE_HEADER);
            println("Log file created: " + new Date());
        } else {
            println();
        }
        println();
    }

    public void notifyProgressEvent(
            Engine engine, int event,
            File src, int pMode,
            Throwable error, Object param) {
        switch (event) {
        case EVENT_BEGIN_PROCESSING_SESSION:
            startTime = System.currentTimeMillis();
            errorCount = 0;
            warningCount = 0;
            executedCount = 0;
            copiedCount = 0;
            println("========================================"
                    + "=======================================");
            println("PROCESSING SESSION STARTED: " + new Date());
            String fmppVersion, freemarkerVersion;
            try {
                fmppVersion = Engine.getVersionNumber();
            } catch (Throwable e) {
                fmppVersion = "??? (" + e + ")";
            }
            try {
                freemarkerVersion = Engine.getFreeMarkerVersionNumber();
            } catch (Throwable e) {
                freemarkerVersion = "??? (" + e + ")";
            }
            println("FMPP version: " + fmppVersion
                    + " (using FreeMarker " + freemarkerVersion + ")");
            
            int startIndex = 0;
            int dotCount = 0;
            while (true) {
                startIndex = fmppVersion.indexOf('.', startIndex);
                if (startIndex == -1) {
                    break;
                }
                startIndex++;
                dotCount++;
            }
            if (dotCount == 3) {
                String buildInfo;
                try {
                    buildInfo = Engine.getBuildInfo();
                } catch (Throwable e) {
                    buildInfo = "??? (" + e + ")";
                }
                println("Unreleased \"nightly\" FMPP version! "
                        + "Version number doesn't reflect changes,");
                println("so watch the build date too: "
                        + buildInfo);
            }
        
            println();
            break;
        case EVENT_WARNING:
            warningCount++;
            println("----------------------------------------"
                    + "---------------------------------------");
            println("*** WARNING: " + (String) param);
            if (src != null) {
                try {
                    println("Source file: " + FileUtil.getRelativePath(
                            engine.getSourceRoot(), src));
                } catch (IOException e) {
                    println("???");
                }
            }
            println();
            break;
        case EVENT_END_PROCESSING_SESSION:
            println("========================================"
                    + "=======================================");
            println("END OF PROCESSING SESSION");
            println();
            
            println("Summary:\n"
                    + executedCount + " executed + "
                    + renderedCount + " xmlRendered + "
                    + copiedCount + " copied = "
                    + (executedCount + copiedCount + renderedCount)
                    + " successfully processed\n"
                    + errorCount + " failed, "
                    + warningCount + " warning(s)");
            println("Time elapsed: "
                    + (System.currentTimeMillis() - startTime) / 1000.0
                    + " seconds");
            break;
        case EVENT_END_FILE_PROCESSING:
            if (error != null) {
                errorCount++;
            } else {
                if (pMode == Engine.PMODE_COPY) {
                    copiedCount++;
                } else if (pMode == Engine.PMODE_EXECUTE) {
                    executedCount++;
                } else if (pMode == Engine.PMODE_RENDER_XML) {
                    renderedCount++;
                }
            }
            break;
        default:
            ; // ignore
        }

        if (error != null) {
            if (event != EVENT_END_PROCESSING_SESSION) {
                println("----------------------------------------"
                        + "---------------------------------------");
                println("!!! ERROR");
            } else {
                println();
                println("WARNING! Processing session was interrupted:");
                println();
            }

            if (error instanceof ProcessingException) {
                src = ((ProcessingException) error).getSourceFile();
            }
            if (src != null) {
                println("> Source file:");
                try {
                    println(FileUtil.getRelativePath(
                            engine.getSourceRoot(),
                            src));
                } catch (IOException e) {
                    println("???");
                }
            }

            println("> Error message:");
            println(MiscUtil.causeMessages(error));
            println("> Java stack trace:");
            error.printStackTrace(out);
            out.flush();

            println();
        }
        if (event == EVENT_END_PROCESSING_SESSION) {
            println("========================================"
                    + "=======================================");
        }
    }
    
    /**
     * Returns the PrintWriter used for printing into the log. 
     */
    public PrintWriter getPrintWriter() {
        return out;
    }

    /**
     * Closes the PrintWriter used for printing into the log.
     */
    public void close() {
        out.close();
    }

    /**
     * Flushes the PrintWriter used for printing into the log.
     */
    public void flush() {
        out.flush();
    }

    /**
     * Prints into the log without word-wrapping. 
     */
    public void printlnNW(String text) {
        out.println(text);
    }

    /**
     * Prints into the log without word-wrapping.
     */
    public void printNW(String text) {
        out.print(text);
    }
    
    /**
     * Prints line-break into the log.
     */
    public void println() {
        out.println();
    }

    /**
     * Prints into the log with word-wrapping.
     */
    public void println(Object obj) {
        println(obj.toString());
    }

    /**
     * Prints into the log with word-wrapping.
     */
    public void println(String text) {
        println(text, 0);
    }

    /**
     * Prints into the log with word-wrapping and indentation.
     */
    public void println(String text, int indent) {
        out.println(StringUtil.wrap(text, 80, indent));
    }

    public void printStackTrace(Throwable exc) {
        printlnNW("--- Java stack trace: ---");
        exc.printStackTrace(out);
        println();
    }        
}
