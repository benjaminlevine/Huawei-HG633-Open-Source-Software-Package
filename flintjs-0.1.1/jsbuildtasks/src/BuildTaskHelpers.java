/*
Copyright 2011 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * A custom Ant task that isolates inline JavaScript tests demarcated with //#BEGIN_TEST and
 * //#END_TEST for all files in the contained FileSets.
 */
public class BuildTaskHelpers {

    /**
     * Reads the entire contents of a file into a string.
     */
    public static String readFileContents(File file) throws IOException {
        StringBuilder builder = new StringBuilder();
        BufferedReader reader = new BufferedReader(new FileReader(file));
        String line;
        Boolean first = true;
        while ((line = reader.readLine()) != null) {
            builder.append((first ? "" : "\n") + line);
            first = false;
        }
        reader.close();
        return builder.toString();
    }

    /**
     * Writes the contents of a string to a file.
     */
    public static void writeFileContents(File file, String contents)
            throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter(file));
        writer.write(contents);
        writer.close();
    }

    /**
     * A private class that handles dependency graph related functionality (i.e. topological
     * sorts).
     */
    public static class DependencyGraph<T> {
        public static class FoundCycleException extends Exception {
        }

        /**
         * An ordered list of nodes (i.e. tasks).
         */
        private List<T> nodes;

        /**
         * A map of a node to its dependencies.
         */
        private Map<T, List<T>> dependencies;

        public DependencyGraph() {
            this.nodes = new ArrayList<T>();
            this.dependencies = new HashMap<T, List<T>>();
        }

        public DependencyGraph(List<T> nodes) {
            this.nodes = nodes;
            this.dependencies = new HashMap<T, List<T>>();
        }

        public DependencyGraph(List<T> nodes, Map<T, List<T>> dependencies) {
            this.nodes = nodes;
            this.dependencies = dependencies;
        }

        public void addNode(T node) {
            if (!nodes.contains(node)) {
                nodes.add(node);
            }
        }

        public void addDependency(T dependentNode, T parentNode) {
            if (!this.dependencies.containsKey(dependentNode)) {
                this.dependencies.put(dependentNode, new ArrayList<T>());
            }

            List<T> parentNodes = this.dependencies.get(dependentNode);
            if (!parentNodes.contains(parentNode)) {
                parentNodes.add(parentNode);
            }
        }

        /**
         * Computes the correct execution order for all of the nodes in the graph. The order
         * returned is stable and initial node order is kept when possible.
         */
        public List<T> computeExecutionOrder() throws FoundCycleException {
            // Algorithm by Kahn, A. B. (1962)
            // http://en.wikipedia.org/wiki/Topological_sorting
            List<T> executionOrder = new ArrayList<T>();

            Map<T, List<T>> dependencies = new HashMap<T, List<T>>();
            for (T dependentNode : this.dependencies.keySet()) {
                List<T> parentNodes = new ArrayList<T>();
                for (T parentNode : this.dependencies.get(dependentNode)) {
                    parentNodes.add(parentNode);
                }
                dependencies.put(dependentNode, parentNodes);
            }

            List<T> nodes = new ArrayList<T>(this.nodes);

            while (true) {
                List<T> freeNodes = new ArrayList<T>(nodes);
                Iterator<T> it = freeNodes.iterator();
                while (it.hasNext()) {
                    T node = it.next();
                    if (dependencies.containsKey(node)) {
                        it.remove();
                    }
                }

                if (freeNodes.isEmpty()) {
                    break;
                }

                T node = freeNodes.remove(0);
                nodes.remove(node);
                executionOrder.add(node);

                Iterator<Map.Entry<T, List<T>>> it2 =
                        dependencies.entrySet().iterator();
                while (it2.hasNext()) {
                    Map.Entry<T, List<T>> entry = it2.next();
                    List<T> parentNodes = entry.getValue();
                    if (parentNodes.contains(node)) {
                        parentNodes.remove(node);
                    }
                    if (parentNodes.isEmpty()) {
                        it2.remove();
                    }
                }
            }

            if (!dependencies.isEmpty()) {
                throw new FoundCycleException();
            }

            return executionOrder;
        }
    }
}
