/*
 * Unitex
 *
 * Copyright (C) 2001-2015 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
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

/*
 * File created and contributed by Gilles Vollant (Ergonotics SAS)
 * as part of an UNITEX optimization and reliability effort
 *
 * additional information: http://www.ergonotics.com/unitex-contribution/
 * contact : unitex-contribution@ergonotics.com
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Unicode.h"
#include "Alphabet.h"
#include "String_hash.h"
#include "File.h"
#include "Copyright.h"
#include "DELA.h"
#include "Error.h"
#include "Vector.h"
#include "HashTable.h"
#include "UnitexGetOpt.h"
#include "Token.h"
#include "Text_tokens.h"
#include "DumpOffsets.h"
#include "Offsets.h"

#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {

const char* usage_DumpOffsets =
         "Usage: DumpOffsets [OPTIONS] <txt>\n"
         "\n"
         "  <txt>: a offset file to read\n"
         "\n"
         "OPTIONS:\n"
         "  -o X/--old=X: name of old file to read\n"
         "  -n X/--new=X: name of new file to read\n"
         "  -p X/--output=X: name of output dump file to write\n"
         "  -f/--full: dump common text additionaly\n"
         "  -q/--quiet: display no message\n"
         "  -c/--no_escape_sequence: don't escape text sequence\n"
         "  -h/--help: this help\n"
         "\n"
         "DumpOffsets dump sequence offset to study them.\n"
         "\n" \
         "Example:\n" \
         "UnitexToolLogger Normalize -r .\\resource\\Norm.txt .\\work\\text_file.txt --output_offsets .\\work\\text_file_offset.txt\n" \
         "UnitexToolLogger DumpOffsets -o .\\work\\text_file_offset.txt -n .\\work\\text_file_offset.snt -p .\\work\\dump\\dump_offsets.txt .\\work\\text_file_offset.txt\n" \
         "\n" \
         "\n" \
         "\n" \
         "Other Usage: DumpOffsets [-m/--merge] [OPTIONS] <txt>\n"
         "\n"
         "  <txt>: a offset file to read\n"
         "\n"
         "OPTIONS:\n"
         "  -o X/--old=X: name of old offset file to read\n"
         "  -p X/--output=X: name of output merged offset file to write\n"
         "Merge two offset file produced by two successive modification of text\n"
         "\n" \
         "\n" \
         "\n" \
         "Other Usage: DumpOffsets [-v/--convert_modified_to_common] [OPTIONS] <txt>\n" \
         "\n" \
         "  <txt>: a offset file to read\n" \
         "\n" \
         "OPTIONS:\n" \
         "  -s N/--old_size=N: size of original file (in characters)\n" \
         "  -S N/--new_size=N: size of modified file (in characters)\n" \
         "  -p X/--output=X: name of output common offset file to write\n" \
         "  -h/--help: this help\n" \
         "Create an offset file which list offset of common string between the original and modified file. At least one size must be provided\n" \
         "\n" \
         "\n" \
         "\n" \
         "Other Usage: DumpOffsets [-V/--convert_common_to_modified] [OPTIONS] <txt>\n" \
         "\n" \
         "  <txt>: a offset file to read\n" \
         "\n" \
         "OPTIONS:\n" \
         "  -s N/--old_size=N: size of original file (in characters)\n" \
         "  -S N/--new_size=N: size of modified file (in characters)\n" \
         "  -p X/--output=X: name of output common offset file to write\n" \
         "  -h/--help: this help\n" \
         "Create a standard modified offset file from offset of common string between the original and modified file. Both size must be provided\n" \
         "\n" \
         "\n" \
         "\n" \
         "Other Usage: DumpOffsets -o <list_of_position_file_to_read.txt> -p <list_to_create> -T <offset_file_to_read>\n" \
         "\n" \
         "  <list_of_position_file_to_read.txt> is a text file with just one number (a position) at each line." \
         "\n" \
         "This will convert a list of position using the offset file\n" \
         "The created file contain the converted position at each line, with a + en end of line if the character\n" \
         "at this position is on result file, a - is it was removed.\n" \
         "Using -t instead -T will do the reverse translation\n" \
         "\n" \
         "\n" \
         ""
         ;

static void usage() {
u_printf("%S",COPYRIGHT);
u_printf(usage_DumpOffsets);
}


const char* optstring_DumpOffsets=":hfumvVtTs:S:o:n:p:k:q:";
const struct option_TS lopts_DumpOffsets[]={
   {"old",required_argument_TS, NULL,'o'},
   {"new",required_argument_TS,NULL,'n'},
   {"output",required_argument_TS,NULL,'p'},
   {"no_escape_sequence",required_argument_TS,NULL,'c'},
   {"merge",no_argument_TS,NULL,'m'},
   {"convert_modified_to_common",no_argument_TS,NULL,'v'},
   {"convert_common_to_modified",no_argument_TS,NULL,'V'},
   {"old_size",required_argument_TS,NULL,'s'},
   {"new_size",required_argument_TS,NULL,'S'},
   {"full",no_argument_TS,NULL,'f'},
   {"quiet",no_argument_TS,NULL,'u'},
   {"input_encoding",required_argument_TS,NULL,'k'},
   {"output_encoding",required_argument_TS,NULL,'q'},
   {"help", no_argument_TS, NULL, 'h'},
   {NULL, no_argument_TS, NULL, 0}
};




#define READ_FILE_BUFFER_SIZE 65536

/**
* This code run well, but in really not optimized
*/
static unichar* read_text_file(U_FILE *f,int * filesize){

    unichar *text = NULL;
    *filesize = 0;

    text = (unichar *)malloc(sizeof(unichar));
    if (text == NULL){
        fatal_alloc_error("read_text_file");
    }
    text[0] = '\0';

    int total_read = 0;
    int read;
    do {
        unichar buffer[READ_FILE_BUFFER_SIZE + 1];
        memset(buffer, 0, sizeof(unichar)*(READ_FILE_BUFFER_SIZE + 1));

        for (read = 0; read < READ_FILE_BUFFER_SIZE; read++)
        {
            int r = u_fgetc_raw(f);
            if (r == EOF)
                break;
            *(buffer + read) = (unichar)r;
        }

        total_read += u_strlen(buffer);
        text = (unichar *)realloc(text, sizeof(unichar)*(total_read + 1));
        if (text == NULL){
            fatal_alloc_error("read_text_file");
        }
        u_strcat(text, buffer);

    } while (read == READ_FILE_BUFFER_SIZE);

    text[total_read] = '\0';
    *filesize = total_read;
    return text;
}


static void read_text_file(const VersatileEncodingConfig* cfg, const char*filename, unichar** buffer, int *filesize)
{
    U_FILE* f = u_fopen(cfg, filename, U_READ);
    if (f == NULL) {
        fatal_error("cannot read file %s", filename);
    }
    *buffer = read_text_file(f, filesize);
    u_fclose(f);
}


static int DumpSequence(U_FILE* f,const unichar* text, int textsize, int start, int end, int escape)
{
    if (end < start)
    {
        u_fprintf(f, "Invalid sequence : end before start !\n");
        return 1;
    }
    else
    if (end > textsize)
    {
        u_fprintf(f, "Invalid sequence : end after end of file !\n");
        return 1;
    }
    else
    if (end == start)
    {
        u_fprintf(f, "empty sequence\n");
        return 0;
    }
    else
    {
        u_fprintf(f, "'");
        for (int i = start; i < end; i++) {
            unichar c = *(text + i);
            if (escape) {
                if (c >= 0x20) {
                    u_fputc(c, f);
                }
                else {
                    switch (c)
                    {
                        case '\r' : u_fprintf(f, "\\r"); break;
                        case '\n': u_fprintf(f, "\\n"); break;
                        case '\t': u_fprintf(f, "\\t"); break;
                        default: u_fprintf(f,"\\x%02x", (unsigned int)c); break;
                    }
                }
            }
            else {
                u_fputc(c, f);
            }
        }
        u_fprintf(f, "'\n");
    }
    return 0;
}



static int CompareCommon(U_FILE* f,
    const unichar*old_text, int old_size, int old_pos, int old_limit,
    const unichar*new_text, int new_size, int new_pos, int new_limit)
{
    if ((old_limit < old_pos) || (new_limit < new_pos)) {
        u_fprintf(f, "Invalid common sequence : end before start !\n");
        return 1;
    } else if ((old_limit > old_size) || (new_limit > new_size)) {
        u_fprintf(f, "Invalid commonsequence : end after end of file !\n");
        return 1;
    } else if ((old_limit - old_pos) != (new_limit - new_pos)) {
        u_fprintf(f, "Invalid common sequence : size mismatch !\n");
        return 1;
    }

    for (int i = 0; i < (old_limit - old_pos); i++) {
        if ((*(old_text + old_pos + i)) != (*(new_text + new_pos + i))) {
            u_fprintf(f, "Difference on common sequence !\n");
            return 1;
        }
    }
    return 0;
}


static void load_offset_translation(const VersatileEncodingConfig* vec, const char* name, int** pos_from_file, int * nb_position) {
    *pos_from_file = NULL;
    *nb_position = 0;
    U_FILE* f = u_fopen(vec, name, U_READ);
    if (f == NULL) return ;

    int nb_allocated = 1;
    *pos_from_file = (int*)malloc(sizeof(int) * nb_allocated);
    if ((*pos_from_file) == NULL) {
        fatal_alloc_error("load_offset_translation");
    }

    int a,n;

    while ((n = u_fscanf(f, "%d", &a)) != EOF) {
        if (n != 1) {
            fatal_error("Corrupted file %s\n", name);
        }
        if ((*nb_position) >= nb_allocated) {
            nb_allocated *= 2;
            *pos_from_file = (int*)realloc(*pos_from_file, sizeof(int) * nb_allocated);
            if ((*pos_from_file) == NULL) {
                fatal_alloc_error("load_offset_translation");
            }
        }
        *((*pos_from_file) + (*nb_position)) = a;
        (*nb_position)++;
    }
    u_fclose(f);
    return ;
}


int DumpOffsetApply(const VersatileEncodingConfig* vec, const char* old_filename, const char* new_filename,
    const char*offset_file_name, const char* output,
    int full, int quiet, int escape)
{
    unichar* old_text = NULL;
    int old_read_size = 0;
    read_text_file(vec, old_filename, &old_text, &old_read_size);

    unichar* new_text = NULL;
    int new_read_size = 0;
    read_text_file(vec, new_filename, &new_text, &new_read_size);

    vector_offset* offsets = load_offsets(vec, offset_file_name);
    if (offsets == NULL) {
        fatal_error("cannot read file %s", offset_file_name);
    }
    int coherency = 1;
    U_FILE* fout = u_fopen(vec, output, U_WRITE);
    for (int i = 0; i < offsets->nbelems; i++) {
        Offsets curOffset = offsets->tab[i];
        Offsets prevOffset;
        if (i > 0) {
            prevOffset = offsets->tab[i - 1];
        }
        else {
            prevOffset.old_end = prevOffset.new_end = 0;
        }
        if (CompareCommon(fout, old_text, old_read_size, prevOffset.old_end, curOffset.old_start,
            new_text, new_read_size, prevOffset.new_end, curOffset.new_start)) {
            coherency = 0;
        }
        if (full) {
            u_fprintf(fout, "===========================================\n\n");
            u_fprintf(fout, "Common zone:\n\n");
            DumpSequence(fout, old_text, old_read_size, prevOffset.old_end, curOffset.old_start, escape);
            DumpSequence(fout, new_text, new_read_size, prevOffset.new_end, curOffset.new_start, escape);
        }
        if ((i > 0) || (full)) {
            u_fprintf(fout, "-------------------------------------------\n\n");
        }
        u_fprintf(fout, "%8d: %d.%d -> %d.%d\n", i, curOffset.old_start, curOffset.old_end, curOffset.new_start, curOffset.new_end);
        if (DumpSequence(fout, old_text, old_read_size, curOffset.old_start, curOffset.old_end, escape)) {
            coherency = 0;
        }
        if (DumpSequence(fout, new_text, new_read_size, curOffset.new_start, curOffset.new_end, escape)) {
            coherency = 0;
        }

        if ((i + 1) == offsets->nbelems) {
            if (CompareCommon(fout, old_text, old_read_size, curOffset.old_end, old_read_size,
                new_text, new_read_size, curOffset.new_end, new_read_size))
            {
                coherency = 0;
            }

            if ((full) && ((curOffset.old_end != old_read_size) || (curOffset.new_end != new_read_size))) {
                u_fprintf(fout, "===========================================\n\n");
                u_fprintf(fout, "Last Common zone:\n\n");
                DumpSequence(fout, old_text, old_read_size, curOffset.old_end, old_read_size, escape);
                DumpSequence(fout, new_text, new_read_size, curOffset.new_end, new_read_size, escape);
            }
        }
    }

    if (offsets->nbelems == 0)
    {
        if (CompareCommon(fout, old_text, old_read_size, 0, old_read_size,
            new_text, new_read_size, 0, new_read_size))
        {
            coherency = 0;
        }

        if ((full) && ((0 != old_read_size) || (0 != new_read_size))) {
            u_fprintf(fout, "===========================================\n\n");
            u_fprintf(fout, "Last Common zone:\n\n");
            DumpSequence(fout, old_text, old_read_size, 0, old_read_size, escape);
            DumpSequence(fout, new_text, new_read_size, 0, new_read_size, escape);
        }
    }

    free_vector_offset(offsets);
    free(old_text);
    free(new_text);
    u_fprintf(fout, "\n\nOffset file is %s.\n", coherency ? "coherent" : "not coherent");
    u_fclose(fout);
    if (!quiet) {
        u_printf("Offset file is %s.\n", coherency ? "coherent" : "not coherent");
        u_printf("\nDumpOffsets dump done, file %s created.\n", output);
    }
    return coherency ? 0 : 1;
}


int main_DumpOffsets(int argc,char* const argv[]) {
if (argc==1) {
   usage();
   return 0;
}

char old_filename[FILENAME_MAX]="";
char new_filename[FILENAME_MAX]="";
char output[FILENAME_MAX]="";
char offset_file_name[FILENAME_MAX]="";
VersatileEncodingConfig vec=VEC_DEFAULT;
int escape=1;
int merge=0;
int full=0;
int quiet=0;
int convert_modified_to_common=0;
int convert_common_to_modified=0;
int translate_position_file=0;
int translate_position_file_invert=0;
int old_size=-1;
int new_size=-1;
int val,index=-1;
//char foo=0;
struct OptVars* vars=new_OptVars();
while (EOF!=(val=getopt_long_TS(argc,argv,optstring_DumpOffsets,lopts_DumpOffsets,&index,vars))) {
   switch(val) {
   case 'o': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty old file name\n");
             }
             strcpy(old_filename, vars->optarg);
             break;
   case 'n': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty new file name\n");
             }
             strcpy(new_filename, vars->optarg);
             break;
   case 'p': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty output file name\n");
             }
             strcpy(output, vars->optarg);
             break;
   case 's': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty size for old file\n");
             }
             old_size = atoi(vars->optarg);
             break;
   case 'S': if (vars->optarg[0]=='\0') {
                fatal_error("You must specify a non empty size for new file\n");
             }
             new_size = atoi(vars->optarg);
             break;
   case 'k': if (vars->optarg[0]=='\0') {
                fatal_error("Empty input_encoding argument\n");
             }
             decode_reading_encoding_parameter(&(vec.mask_encoding_compatibility_input),vars->optarg);
             break;
   case 'q': if (vars->optarg[0]=='\0') {
                fatal_error("Empty output_encoding argument\n");
             }
             decode_writing_encoding_parameter(&(vec.encoding_output),&(vec.bom_output),vars->optarg);
             break;
   case 'c': escape = 0; break;
   case 'f': full = 1; break;
   case 'm': merge = 1; break;
   case 'v': convert_modified_to_common = 1; break;
   case 'V': convert_common_to_modified = 1; break;
   case 't': translate_position_file = 1; break;
   case 'T': translate_position_file_invert = 1; break;
   case 'u': quiet = 1; break;
   case 'h': usage(); return 0;
   case ':': if (index==-1) fatal_error("Missing argument for option -%c\n",vars->optopt);
             else fatal_error("Missing argument for option --%s\n",lopts_DumpOffsets[index].name);
   case '?': if (index==-1) fatal_error("Invalid option -%c\n",vars->optopt);
             else fatal_error("Invalid option --%s\n",vars->optarg);
             break;
   }
   index=-1;
}

if (vars->optind!=argc-1) {
   fatal_error("Invalid arguments: rerun with --help\n");
}


strcpy(offset_file_name, argv[vars->optind]);
free_OptVars(vars);

if (translate_position_file || translate_position_file_invert) {
    int* pos_from_file = NULL;
    int nb_translations = 0;
    load_offset_translation(&vec, old_filename, &pos_from_file, &nb_translations);
    vector_offset* modified_offset = load_offsets(&vec, offset_file_name);
    offset_translation* translation = (offset_translation*)malloc(sizeof(offset_translation) * (nb_translations + 1));
    for (int i = 0;i < nb_translations;i++) {
        (translation + i)->position_to_translate = *(pos_from_file + i);
        (translation + i)->sort_order = i;
    }
    free(pos_from_file);
    translate_offset(translation, nb_translations, modified_offset, translate_position_file_invert ? 1 : 0);
    free_vector_offset(modified_offset);

    U_FILE* f_output_translation = u_fopen(&vec, output, U_WRITE);
    for (int i = 0;i < nb_translations;i++) {
        u_fprintf(f_output_translation, "%d %s\n",
            (translation + i)->translated_position,
            ((translation + i)->translation_pos_in_common == -1) ? "-" : "+");
    }
    u_fclose(f_output_translation);
    free(translation);
    return 0;
} else if (convert_modified_to_common) {
    int ret_value = 1;
    vector_offset* modified_offset = NULL;
    if ((old_size == -1) && (new_size == -1)) {
        error("you must specify old or new filesize");
    }
    else {
        modified_offset = load_offsets(&vec, offset_file_name);
    }
    if (modified_offset == NULL) {
        error("error in reading offset file %s\n", offset_file_name);
    } else {
        vector_offset* common_offset = modified_offsets_to_common(modified_offset, old_size, new_size);
        if (common_offset == NULL) {
            error("invalid offset conversion from file %s with file size from %d to %d\n", old_size, new_size);
        }
        else {
            save_offsets(&vec, output, common_offset);
            free_vector_offset(common_offset);
            ret_value=0;
        }
        free_vector_offset(modified_offset);
    }
    return ret_value;
} else if (convert_common_to_modified) {
    int ret_value = 1;
    vector_offset* common_offset = NULL;
    if ((old_size == -1) && (new_size == -1)) {
        error("you must specify old and new filesize");
    }
    else {
        common_offset = load_offsets(&vec, offset_file_name);
    }
    if (common_offset == NULL) {
        error("error in reading offset file %s\n", offset_file_name);
    }
    else {
        vector_offset* modified_offset = common_offsets_to_modified(common_offset, old_size, new_size);
        if (modified_offset == NULL) {
            error("invalid offset conversion from file %s with file size from %d to %d\n", old_size, new_size);
        }
        else {
            save_offsets(&vec, output, modified_offset);
            free_vector_offset(modified_offset);
            ret_value=0;
        }
        free_vector_offset(common_offset);
    }
    return ret_value;
} else if (merge) {
    vector_offset* prev_offsets = load_offsets(&vec, old_filename);
    vector_offset* offsets = load_offsets(&vec, offset_file_name);

    U_FILE* f_output_offsets = u_fopen(&vec, output, U_WRITE);
    if (f_output_offsets == NULL) {
        error("Cannot create offset file %s\n", output);
        return 2;
    }
    process_offsets(prev_offsets, offsets, f_output_offsets);
    free_vector_offset(prev_offsets);
    free_vector_offset(offsets);
    u_fclose(f_output_offsets);
    if (!quiet) {
        u_printf("\nDumpOffsets dump done, file %s created.\n", output);
    }
    return 0;
} else {
    return DumpOffsetApply(&vec, old_filename, new_filename, offset_file_name, output,
        full, quiet, escape);
}


}

} // namespace unitex