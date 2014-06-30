/* hostlist_ipx.c   2004 Ian Schorr
 * modified from endpoint_talkers_ipx.c   2003 Ronnie Sahlberg
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include <string.h>

#include <gtk/gtk.h>

#include "epan/packet.h"
#include <epan/stat_cmd_args.h>
#include <epan/tap.h>
#include <epan/dissectors/packet-ipx.h>

#include "ui/stat_menu.h"

#include "ui/gtk/gui_stat_menu.h"
#include "ui/gtk/hostlist_table.h"

void register_tap_listener_ipx_hostlist(void);

static int
ipx_hostlist_packet(void *pit, packet_info *pinfo, epan_dissect_t *edt _U_, const void *vip)
{
	hostlist_table *hosts=(hostlist_table *)pit;
	const ipxhdr_t *ipxh=(const ipxhdr_t *)vip;

	/* Take two "add" passes per packet, adding for each direction, ensures that all
	packets are counted properly (even if address is sending to itself)
	XXX - this could probably be done more efficiently inside hostlist_table */
	add_hostlist_table_data(hosts, &ipxh->ipx_src, 0, TRUE, 1, pinfo->fd->pkt_len, SAT_NONE, PT_NONE);
	add_hostlist_table_data(hosts, &ipxh->ipx_dst, 0, FALSE, 1, pinfo->fd->pkt_len, SAT_NONE, PT_NONE);

	return 1;
}



static void
gtk_ipx_hostlist_init(const char *opt_arg, void* userdata _U_)
{
	const char *filter=NULL;

	if(!strncmp(opt_arg,"hosts,ipx,",10)){
		filter=opt_arg+10;
	} else {
		filter=NULL;
	}

	init_hostlist_table(TRUE, "IPX", "ipx", filter, ipx_hostlist_packet);

}

void
gtk_ipx_hostlist_cb(GtkAction *action _U_, gpointer user_data _U_)
{
	gtk_ipx_hostlist_init("hosts,ipx",NULL);
}

void
register_tap_listener_ipx_hostlist(void)
{
	register_stat_cmd_arg("hosts,ipx", gtk_ipx_hostlist_init,NULL);
	register_hostlist_table(TRUE, "IPX", "ipx", NULL /*filter*/, ipx_hostlist_packet);
}
