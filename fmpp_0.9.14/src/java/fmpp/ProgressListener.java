package fmpp;

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

/**
 * Interface to monitor the events of an <code>Engine</code>.
 * 
 * <p>The object that implements this interface need to be plugged into the
 * {@link Engine} to receive notifications about its events. For example, if you
 * add an object as engine attribute or as local data loader to the engine, it
 * will be notified about the events. Object that has no role other than
 * listening to events can to be plugged with
 * {@link Engine#addProgressListener}.
 * 
 * <p>Examples of the usage of progress listeners:
 * <ul>
 *   <li>Displaying visually the progress of the processing session for the
 *       user.
 *   <li>Creating statistics about the processing session.
 *   <li>Implementing a poor man's logging (until no logging support added to
 *       FMPP)
 *   <li>Releasing the resources held by local data loaders and engine
 *       attributes when the session is finished.
 * </ul>
 */
public interface ProgressListener {
    /** <code>Engine.process</code> has started the work. */
    int EVENT_BEGIN_PROCESSING_SESSION = 1;

    /** <code>Engine.process</code> has finished the work. */
    int EVENT_END_PROCESSING_SESSION = 4;
    
    /** The processing of a single file has been started. */
    int EVENT_BEGIN_FILE_PROCESSING = 2;
    
    /** The processing of a single file has been finished. */
    int EVENT_END_FILE_PROCESSING = 3;
    
    /** A source directory has been ignored (skipped.) */
    int EVENT_IGNORING_DIR = 5;

    /** An empty directory was created due to the copyEmptyDirs setting. */
    int EVENT_CREATED_EMPTY_DIR = 8;
    
    /**
     * A warning message has been received from a template or from the
     * engine.
     */
    int EVENT_WARNING = 6;
    
    /**
     * The processing of source was skipped because the output was generated
     * after the last modification of the source file.
     */
    int EVENT_SOURCE_NOT_MODIFIED = 7;
    
    /**
     * Method called be the engine to notify events.
     *
     * <p>It is guaranteed that this method will not be called concurrently
     * as far as the listener is added to a single <code>Engine</code> instance
     * only. 
     *   
     * @param engine The engine instance where the event has occurred.
     * @param event The code of the event: an <code>EVENT_...</code> constant.
     *     As new event types can be introduced with new FMPP versions (even if
     *     it happens very seldom), a progress listener implementation should
     *     survive events that it does not understand. That is, it must not stop
     *     with an error, but it should silently ignore the event.
     * @param src Depending on <code>event</code> the source file or null.
     * @param pMode Depending on <code>event</code> the proccessing mode
     *     (<code>Engine.PMODE_...</code> constant) or
     *     <code>Engine.PMODE_NONE</code>. Note that new processing modes may
     *     be added as FMPP evolvers, so values that are not known be the
     *     progress listener should be handler nicely, and never cause error.  
     * @param error The error, or null if there was no error.
     * @param param Extra information about the event. The class and meaning of
     *     object depends on the concrete event:
     *     <ul>
     *       <li>For <code>EVENT_WARNING</code> it is a <code>String</code> that
     *           describles the reason of warning.
     *     </ul>
     */
    void notifyProgressEvent(
            Engine engine,
            int event,
            File src, int pMode,
            Throwable error, Object param) throws Exception;
}
