import argparse
import sys
import subprocess

parser = argparse.ArgumentParser()

parser.add_argument("-q", "--qt", choices=['5.9.1', '5.12.1', '5.14.2'], required=True, help="Specify Qt version to use")
parser.add_argument("-g", "--git-tag", required=True, help='Git tag to be fetched and built')
parser.add_argument("-d", "--dist-version", required=True, help='version number to be attached to dist package')
parser.add_argument("-s", "--stim-version", required=True, help='version number to be attached to stim package')
#parser.add_argument("-m", "--min-os-version", required=True, help='os min version e.g. 101000 = 10.10')
#parser.add_argument("-x", "--max-os-version", help='os max version e.g. 101000 = 10.10')
parser.add_argument("-e", "--extra-label", required=True, help='appended to package basename: habit2-<git-tag><extra-label>.pkg')
parser.add_argument("-t", "--display-title", required=True, help="Title at top of installer window")
parser.add_argument("-m", "--make-target", default='dmg', required=False, help="make target to build")
args = parser.parse_args()
argsdict = vars(args)
    
print('Got qt %s' % argsdict['qt'])
print('Git tag %s' % argsdict['git_tag'])
print('Dist version %s' % argsdict['dist_version'])
print('Stim version %s' % argsdict['stim_version'])
print('Installer display title %s' % argsdict['extra_label'])
print('Make target is %s' % argsdict['make_target'])


# set gstreamer-related env vars
# only using 1.12
pkg_config_path='/Library/Frameworks/GStreamer.framework/Versions/Current/Libraries/pkgconfig'
gstreamer_libs_relocated='/Users/dan/gstreamer-1.12.4-relocated.tar'

# signing key to be used for app and for dmg
appSigID = 'Developer ID Application: University of California- Davis (WWAFVH26T8)'

# set env for calling make below. Only set vars that come from REQUIRED vars above.
# optional vars and selections handled below

env={"PATH": "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", \
     "GIT_TAG": argsdict['git_tag'], \
     "DIST_VERSION": argsdict['dist_version'], \
     "STIM_VERSION": argsdict['stim_version'], \
     "PKG_CONFIG_PATH": pkg_config_path, \
     "GSTREAMER_LIBS_RELOCATED": gstreamer_libs_relocated, \
     "SIGID" : appSigID}

# Set env var for qt -- this determines qt version used in dist pkg

qt_bin=''
if args.qt == '5.9.1':
    env['QT_BIN'] = '/Users/dan/Qt/5.9.1/clang_64/bin'
    env['OSMIN'] = '101100'
    env['OSMAX'] = '101299'
elif args.qt == '5.12.1':
    env['QT_BIN'] = '/Users/dan/Qt/5.12.1/5.12.1/clang_64/bin'
    env['OSMIN'] = '101300'
elif args.qt == '5.14.2':
    env['QT_BIN'] = '/Users/dan/Qt/5.14.2/5.14.2/clang_64/bin'
    env['OSMIN'] = '101300'
else:
    print('Unrecognized qt target %s' % args.qt)
    sys.exit(1)
    

# extra label for package name - required arg
env['EXTRA_LABEL'] = args.extra_label;

# Word(s) to display in installer dialog title: Installing "words"
env['DISPLAY_TITLE'] = args.display_title


# now run make
cp=subprocess.run(["make", "-f", "reloc-macdist.mak", argsdict['make_target']], env=env)
sys.exit(cp.returncode)