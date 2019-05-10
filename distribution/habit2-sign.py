#!/usr/bin/env python3

import os, sys, glob, subprocess
from pip._vendor.appdirs import AppDirs

if len(sys.argv) < 2:
    print('error - no app dir provided on command line')
    exit
    

appDir = sys.argv[1]
appSigID = 'Developer ID Application: University of California- Davis (WWAFVH26T8)'

if not os.path.isdir(appDir):
    print('error - app dir not found')



# generator that returns filenames (not dirs) inside the passed dir. Will recurse the folder!

def justFiles(dir):
    if os.path.isdir(dir):
        for dirName, subdirList, fileList in os.walk(dir):
            if len(fileList) > 0:
                for f in fileList:
                    yield os.path.join(dirName, f)

# generator that returns frameworks inside the given path

def frameworks(dir):
    fwGlob=glob.glob(os.path.join(dir, '*.framework'))
    for f in fwGlob:
        yield(f)

def signThis(sigID, fileOrFramework):
    print('signing with sigID %s' % sigID)
    subprocess.call(['codesign', '--verbose', '-s', sigID, fileOrFramework])




      
print('Sign frameworks...')  
for fw in frameworks(os.path.join(appDir, 'Contents/Frameworks')):
    print('framework %s' % fw)
    signThis(appSigID, fw)
    
print('Sign plugins...')
for f in justFiles(os.path.join(appDir, 'Contents/PlugIns')):
    print('plugin %s' % f)
    signThis(appSigID, f)
    
print('Sign resources...')
for f in justFiles(os.path.join(appDir, 'Contents/Resources')):
    print('resource %s' % f)
    signThis(appSigID, f)
    
# print('Sign executable...')
# for f in justFiles(os.path.join(appDir, 'Contents/MacOS')):
#     print(f)
#     signThis(appSigID, f)

print('Sign bundle...(this includes executable)...')
signThis(appSigID, appDir)