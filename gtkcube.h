/*
 * gtkcube.h
 *
 * by Joern Thyssen <jthyssen@dk.ibm.com>, 2002
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 or later of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: gtkcube.h,v 1.11 2013/06/16 02:16:14 mdpetch Exp $
 */

#ifndef GTKCUBE_H
#define GTKCUBE_H

extern GtkWidget *CreateCubeAnalysis(moverecord * pmr, const matchstate * pms, int did_double, int did_take, int hist);

#endif
