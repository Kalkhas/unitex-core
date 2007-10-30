 /*
  * Unitex
  *
  * Copyright (C) 2001-2007 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  * 
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Unicode.h"
#include "Copyright.h"
#include "ConcordMain.h"
#include "IOBuffer.h"


void usage() {
u_printf("%S",COPYRIGHT);
u_printf("Usage: Concord <concor> <font> <fontsize> <l> <r> <order> <mode> <alph> [-thai]\n"
       "   <concord> : the concord.ind file generated by the locate program\n"
       "   <font> : the font you want to use\n"
       "   <fontsize> : the font size\n"
       "   <l> : left context length in chars\n"
       "   <r> : right context length in chars\n"
       "       (add \"s\" to right or left context length, e.g. \"80s\",\n"
       "        to stop at first \"{S}\")\n"
       "   <order> : sort order:\n"
       "           TO : text order\n"
       "           LC : left, center\n"
       "           LR : left, right\n"
       "           CL : center, left\n"
       "           CR : center, right\n"
       "           RL : right, left\n"
       "           RC : right, center\n"
       "           NULL : specifies no order (used for non-html <mode>)\n"
       "   <mode> : the mode to be used:\n"
       "            html : produces an HTML concordance file\n"
       "            text : produces a plain text concordance file\n"
       "            glossanet=<script> : produces a glossanet HTML concordance file\n"
       "            index : produces an index of the concordance\n"
       "            uima : produces another index of the concordance\n"
       "            axis : produces an axis file for the concordance (cf. [Melamed 06])\n"
       "            xalign : produces an index file for XAlign display\n"
       "            (txt) : produces a file named (txt) which is the SNT file\n"
       "                    merged with the match results\n"
       "   <alph> : the char order file used for sorting\n"
       "            NULL if no alphabet is given\n"
       "   [-thai] : option to use for thai concordances\n"
       "\nExtracts the matches stored in <concor>, and stores them into a UTF-8\n"
       "HTML file saved in the <concor> directory or produces a text file, according\n"
       "to the <mode> parameter\n"
       "\nExamples:\n"
       "Concord tutu.ind \"Courier New\" 12 40 40 TO html \"d:\\My dir\\alph.txt\"\n"
       "    -> produces an HTML file\n"
       "\n"
       "Concord tutu.ind NULL 0 0 0 NULL \"C:\\My dir\\RES.SNT\" NULL\n"
       "    -> produces a text file named \"C:\\My dir\\RES.SNT\"\n");
}


int main(int argc, char **argv) {
/* Every Unitex program must start by this instruction,
 * in order to avoid display problems when called from
 * the graphical interface */
setBufferMode();
if (argc!=9 && argc!=10 && argc!=11) {
	usage();
	return 0;
}
/* We call an artificial main function located in 'ConcordMain'. This
 * trick allows to use the functionalities of the 'Concord' program
 * without having to launch an external process.
 */
return main_Concord(argc,argv);
}
