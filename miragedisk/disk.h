/*  vim: set noexpandtab ai ts=4 sw=4 tw=4:

    miragedisk, part of a set of disk tools for the Ensoniq Mirage
	(C) 2012 Gordon JC Pearce MM0YEQ <gordon@gjcp.net>
	
	disk.h
	Floppy disk ioctl routine headers

	miragedisk is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	any later version.

	miragedisk is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with miragedisk.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DISK_H
#define __DISK_H

#define MFD_READ 0
#define MFD_WRITE 1

void fd_recalibrate(int fd);
void fd_seek(int fd, int track);
void fd_seekin(int fd);
void fd_readwrite(int fd, int rdwr, int trk, int sect, int len, char *buffer);


#endif
