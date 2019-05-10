#!/usr/local/bin/python3

# import xml.etree.ElementTree as ET
#  
# tree = ET.parse('xmlparsetest.pkgproj')
# root = tree.getroot()
#  
# print('fancy loop')
# [elem.tag for elem in root.iter()]
#  
# print('root element: %s %s: %s' % (root.tag, root.attrib, root.text))
# for child in root:
#     print(child.tag, child.attrib, child.text)
#     for g in child:
#         print('g - %s %s: %s' % (g.tag, g.attrib, g.text))
#          
# #search

import plistlib
import argparse
import sys

isVerbose = False

# minvalue should be a string...why?
def set_os_min(pl, minvalue):
    proj = pl['PROJECT']
    req = proj['PROJECT_REQUIREMENTS']
     
    # reqlist is an array, with one element, that is a dict
    reqlist = req['LIST']
    reqlistdict = reqlist[0]
     
    # in that list...
    print(reqlistdict['BEHAVIOR'])
     
    # there's a dict called DICTIONARY
    reqlistdictdict = reqlistdict['DICTIONARY']
     
    # in that dict lay what we seek
    # assign value
    reqlistdictdict['IC_REQUIREMENT_OS_MINIMUM_VERSION'] = minvalue

    return

# maxvalue should be a string
def set_os_max(pl, maxvalue):
    proj = pl['PROJECT']
    req = proj['PROJECT_REQUIREMENTS']
     
    # reqlist is an array, with one element, that is a dict
    reqlist = req['LIST']
    reqlistdict = reqlist[0]
     
    # in that list...
    print(reqlistdict['BEHAVIOR'])
     
    # there's a dict called DICTIONARY
    reqlistdictdict = reqlistdict['DICTIONARY']
     
    # in that dict lay what we seek
    # assign value
    reqlistdictdict['IC_REQUIREMENT_OS_MAXIMUM_VERSION'] = maxvalue

    return

# maxvalue should be a string
def set_pkg_file_basename(pl, basename):
    proj = pl['PROJECT']
    projsettingsdict = proj['PROJECT_SETTINGS']
    projsettingsdict['NAME'] = basename
    return

def set_versions(pl, distVersion, stimVersion):
    pkgs = pl['PACKAGES']
    for d in pkgs:
        pkgsettingsdict = d['PACKAGE_SETTINGS']
        if pkgsettingsdict['VERSION'] == 'DIST-VERSION':
            pkgsettingsdict['VERSION'] = distVersion
            if isVerbose:
                print('set DIST-VERSION = ', distVersion)
        elif pkgsettingsdict['VERSION'] == 'STIM-VERSION':
            pkgsettingsdict['VERSION'] = stimVersion
            if isVerbose:
                print('set STIM-VERSION = ', stimVersion)
    return

def set_stim_version(pl, version):
    pkgs = pl['PACKAGES']
    for d in pkgs:
        print("got a d!")
    return

def set_display_title(pl, t):
    proj = pl['PROJECT']
    projpresdict = proj['PROJECT_PRESENTATION']
    titledict = projpresdict['TITLE']
    localizationsarray = titledict['LOCALIZATIONS']
    englishdict = localizationsarray[0]
    englishdict['VALUE'] = t
    return


parser = argparse.ArgumentParser()
parser.add_argument("template", help='pkjproj file input')
parser.add_argument("output", help='output filename, will overwrite')
parser.add_argument("-d", "--dist-version", required=True, help="version that pkg-util will report for habit")
parser.add_argument("-s", "--stim-version", required=True, help="version that pkg-util will report for habit-stim")
parser.add_argument("-p", "--pkg-file-basename", required=True, help="pkg file created is basename.pkg")
parser.add_argument("-m", "--min-os-version", help='os min version e.g. 101000 = 10.10')
parser.add_argument("-x", "--max-os-version", help='os max version e.g. 101200 = 10.12')
parser.add_argument("-t", "--display-title", help="Title at top of installer window")
parser.add_argument("-v", "--verbose", action='store_true', default=False, help="verbose me please")
args = parser.parse_args()
argsdict = vars(args)
print(argsdict)

with open(argsdict['template'], 'rb') as fp:
    pl = plistlib.load(fp)
 
 
# did we get verbose?
isVerbose = argsdict['verbose']
 
# Look for min os version, max os version, apply if present. If not present, do nothing
if argsdict['min_os_version'] is not None:
    if isVerbose:
        print('min_os_version found: ', argsdict['min_os_version'])
    set_os_min(pl, argsdict['min_os_version'])
else:
    if isVerbose:
        print('min_os_version not found') 
 
if argsdict['max_os_version'] is not None:
    if isVerbose:
        print('max_os_version found: ', argsdict['max_os_version'])
    set_os_max(pl, argsdict['max_os_version'])
else:
    if isVerbose:
        print('max_os_version not found') 
 
# If pkg file name provided, use it. If not, create one.
pkgName = "habit-installer"
if argsdict['pkg_file_basename'] is not None:
    pkgName = argsdict['pkg_file_basename']
    if isVerbose:
        print('pkg_file_basename: ', argsdict['pkg_file_basename'])
    set_pkg_file_basename(pl, argsdict['pkg_file_basename'])
else:
    if isVerbose:
        print('Warning: using default package name=', pkgName)

# display title for installer
displayTitle = "Habit"
if argsdict['display_title'] is not None:
    displayTitle = argsdict['display_title']
    if isVerbose:
        print('display_title: ', argsdict['display_title'])
else:
    if isVerbose:
        print('Warning: using default display title=', displayTitle)
set_display_title(pl, displayTitle)


# Set DIST-VERSION, STIM-VERSION
# TODO: Assert presence, or rely on parser? 
# I rely on parser now

set_versions(pl, argsdict['dist_version'], argsdict['stim_version'])


with open(argsdict['output'], 'wb') as fpout:
    plistlib.dump(pl, fpout)


