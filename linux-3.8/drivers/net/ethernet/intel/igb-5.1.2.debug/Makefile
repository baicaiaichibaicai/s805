################################################################################
#
# Intel(R) Gigabit Ethernet Linux driver
# Copyright(c) 2007-2013 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#
# The full GNU General Public License is included in this distribution in
# the file called "COPYING".
#
# Contact Information:
# e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
# Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
#
################################################################################

###########################################################################

FAMILYC = e1000_82575.c e1000_i210.c
FAMILYH = e1000_82575.h e1000_i210.h

# core driver files
CFILES = igb_main.c $(FAMILYC) e1000_mac.c e1000_nvm.c e1000_phy.c \
	 e1000_manage.c igb_param.c igb_ethtool.c kcompat.c e1000_api.c \
	 e1000_mbx.c igb_vmdq.c igb_procfs.c igb_hwmon.c igb_debugfs.c
HFILES = igb.h e1000_hw.h e1000_osdep.h e1000_defines.h e1000_mac.h \
	 e1000_nvm.h e1000_manage.h $(FAMILYH) kcompat.h e1000_regs.h \
	 e1000_api.h igb_regtest.h e1000_mbx.h igb_vmdq.h

obj-$(CONFIG_IGB) := $(CFILES:.c=.o)

