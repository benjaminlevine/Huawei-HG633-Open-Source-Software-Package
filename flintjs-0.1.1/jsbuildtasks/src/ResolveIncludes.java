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
 * A custom Ant task that preprocesses //#INSERTFILE and //#ECHO instructions in the contained
 * FileSets (should be all JavaScript files).
 */
public class ResolveIncludes extends org.apache.tools.ant.Task {
    private List<FileSet> fileSets = new ArrayList<FileSet>();

    public void addFileSet(FileSet fileSet) {
        fileSets.add(fileSet);
    }

    /**
     * For resolving relative paths in includes.
     */
    private File origDir;

    public void setOrigdir(String origDir) {
        this.origDir = new File(origDir);
    }

    public void execute() {
        Pattern insP = Pattern.compile("//#INSERTFILE \"(.*?)\"\\n?");
        Pattern echP = Pattern.compile("//#ECHO (.*?)\\n?");

        try {
            for (FileSet fileSet : fileSets) {
                DirectoryScanner ds = fileSet.getDirectoryScanner(getProject());
                File dir = ds.getBasedir();
                String[] filesInSet = ds.getIncludedFiles();

                for (String filename : filesInSet) {
                    File srcFile = new File(dir, filename).getCanonicalFile();
                    File origSrcFile =
                            new File(this.origDir, filename).getCanonicalFile();

                    String src = BuildTaskHelpers.readFileContents(srcFile);

                    // Look for and process //#INSERTFILE directives
                    File origBaseDir = origSrcFile.getParentFile();
                    Matcher insM = insP.matcher(src);
                    StringBuilder builder = new StringBuilder();
                    int appendPos = 0;
                    while (insM.find()) {
                        String includeContents =
                                BuildTaskHelpers.readFileContents(
                                        new File(origBaseDir, insM.group(1)).getCanonicalFile());

                        builder.append(src.substring(appendPos, insM.start()));
                        builder.append(includeContents);
                        appendPos = insM.end();
                    }
                    builder.append(src.substring(appendPos));
                    src = builder.toString();

                    // Look for and process //#ECHO directives
                    Matcher echM = echP.matcher(src);
                    src = echM.replaceAll("$1");

                    // Write output
                    BuildTaskHelpers.writeFileContents(srcFile, src);
                }
            }
        } catch (IOException e) {
            getProject().log(e.toString());
        }
    }
}
