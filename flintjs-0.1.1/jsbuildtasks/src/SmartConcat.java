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

import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.types.FileSet;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * A custom Ant task that preprocesses //#REQUIRE instructions in the contained FileSets (should be
 * all JavaScript files) and concatenates the files in the correct order. If _header.js or
 * _footer.js files are encountered, they are treated as the header/footer for their directories.
 */
public class SmartConcat extends org.apache.tools.ant.Task {
    private List<FileSet> fileSets = new ArrayList<FileSet>();

    public void addFileSet(FileSet fileSet) {
        fileSets.add(fileSet);
    }

    /**
     * For resolving relative paths in includes.
     */
    private File destFile;

    public void setDestfile(String destFile) {
        this.destFile = new File(destFile);
    }

    public void execute() {
        Pattern p = Pattern.compile("//#REQUIRE \"(.*?)\"\\n?");

        BuildTaskHelpers.DependencyGraph<File> dependencyGraph =
                new BuildTaskHelpers.DependencyGraph<File>();
        List<File> srcFiles = new ArrayList<File>();

        try {
            for (FileSet fileSet : fileSets) {
                DirectoryScanner ds = fileSet.getDirectoryScanner(getProject());
                File dir = ds.getBasedir();
                String[] filesInSet = ds.getIncludedFiles();

                for (String filename : filesInSet) {
                    File srcFile = new File(dir, filename).getCanonicalFile();

                    dependencyGraph.addNode(srcFile);
                    srcFiles.add(srcFile);

                    String src = BuildTaskHelpers.readFileContents(srcFile);

                    // Look for and process //#REQUIRE directives
                    File baseDir = srcFile.getParentFile();
                    Matcher m = p.matcher(src);
                    while (m.find()) {
                        File reqFile = new File(baseDir, m.group(1)).getCanonicalFile();
                        dependencyGraph.addNode(reqFile);
                        dependencyGraph.addDependency(srcFile, reqFile);
                    }
                    m.reset();
                    BuildTaskHelpers.writeFileContents(srcFile, m.replaceAll(""));
                }
            }

            // Create implied _header.js and _footer.js dependencies.
            for (File specialFile : srcFiles) {
                SpecialFileKind specialKind = getSpecialFileKind(specialFile);
                if (specialKind == null) {
                    continue;
                }

                String path = specialFile.getParent();
                for (File srcFile : srcFiles) {
                    // Don't let special files depend on special files, otherwise
                    // there will be a circular reference.
                    if (getSpecialFileKind(srcFile) != null) {
                        continue;
                    }

                    if (srcFile.toString().startsWith(path)) {
                        if (specialKind == SpecialFileKind.HEADER) {
                            dependencyGraph.addDependency(srcFile, specialFile);
                        } else if (specialKind == SpecialFileKind.FOOTER) {
                            dependencyGraph.addDependency(specialFile, srcFile);
                        }
                    }
                }
            }

            // Now actually concat the files.
            StringBuilder concatBuilder = new StringBuilder();
            for (File file : dependencyGraph.computeExecutionOrder()) {
                concatBuilder.append(BuildTaskHelpers.readFileContents(file) + "\n");
            }
            BuildTaskHelpers.writeFileContents(destFile, concatBuilder.toString());
        } catch (BuildTaskHelpers.DependencyGraph.FoundCycleException e) {
            getProject().log("Found a circular dependency in the sources files.");
        } catch (IOException e) {
            getProject().log(e.toString());
        }
    }

    private enum SpecialFileKind {HEADER, FOOTER}

    private SpecialFileKind getSpecialFileKind(File file) {
        if (file.getName().equals("_header.js")) {
            return SpecialFileKind.HEADER;
        } else if (file.getName().equals("_footer.js")) {
            return SpecialFileKind.FOOTER;
        }

        return null;
    }
}
