from apport.hookutils import *
import os

def add_info(report, ui=None):
    if not apport.packaging.is_distro_package(report['Package'].split()[0]):
        report['ThirdParty'] = 'True'
        report['CrashDB'] = 'inyokaedit'
        attach_file_if_exists(report, os.getenv("HOME") + '/.InyokaEdit/Debug.log', key='Debug.log')
        attach_file_if_exists(report, os.getenv("HOME") + '/.InyokaEdit/Trolltech.conf', key='Trolltech.conf')
