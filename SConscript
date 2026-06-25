#
# RT-Thread package entry for eRPC.
#
# SPDX-License-Identifier: BSD-3-Clause
#

from building import *
import os

cwd = GetCurrentDir()
group = SConscript(os.path.join(cwd, 'rtthread', 'SConscript'))

Return('group')