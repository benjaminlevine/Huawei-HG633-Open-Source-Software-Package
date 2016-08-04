package fmpp.dataloaders;

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

import java.util.List;

import org.apache.tools.ant.Task;

import fmpp.Engine;
import fmpp.tdd.DataLoader;
import fmpp.util.RuntimeExceptionCC;

/**
 * Abstract base class of Ant related data loaders.
 */
public abstract class AntDataLoader implements DataLoader {
    /**
     * The name of the {@link Engine} attribute that must store the Ant task
     * object.
     */
    public static final String ATTRIBUTE_ANT_TASK = "fmpp.ant.task";
    
    /**
     * Gets the Ant task object, and invoked {@link #load(Engine, List, Task)}.
     */
    public Object load(Engine eng, List args) throws Exception {
        Task task = (Task) eng.getAttribute(ATTRIBUTE_ANT_TASK);
        if (task == null) {
            throw new RuntimeExceptionCC(
                "Ant environment not available: " + ATTRIBUTE_ANT_TASK
                + " Engine attribute is not set.");
        }

        return load(eng, args, task);
    }
    
    /**
     * Override this method to implement your Ant related data loader.
     */
    protected abstract Object load(Engine eng, List args, Task task)
            throws Exception; 
}
