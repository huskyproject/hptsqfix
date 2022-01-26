/*
 *      hptsqfix - rebuilding squish index and some info in sqd
 *      made by Serguei Revtov 2:5021/19.1
 *      from hptUtil by Fedor Lizunkov 2:5020/960@Fidonet
 *
 * This file is part of HPTSQFIX, part of The HUSKY Fidonet Software project.
 * About HUSKY see http://husky.sourceforge.net (http://husky.sf.net)
 *
 * HPTSQFIX is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * HPTSQFIX is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPTSQFIX; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * See also http://www.gnu.org
 *****************************************************************************
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
/* compiler.h */
#include <huskylib/compiler.h>

#if defined (HAS_IO_H)
# include <io.h>
#endif

#if defined (HAS_SHARE_H)
# include <share.h>
#endif

#if defined (HAS_UNISTD_H)
# include <unistd.h>
#endif
/* smapi */
#include <smapi/msgapi.h>

#include "squish.h"
#include "version.h"

char * versionStr;

#define fop_wpb (O_CREAT | O_TRUNC | O_RDWR | O_BINARY)
#define fop_rpb (O_RDWR | O_BINARY)

int tryfind  = 0;
int unDelete = 0;
size_t maxMsgLen;
/*
   typedef struct {
    time_t msgTime;
    dword frame_pos;
   }   sortedbase;

   sortedbase *SortedBase = NULL;
 */
int Open_File(char * name, word mode)
{
    int handle;

#ifdef __UNIX__
    handle = sopen(name,
                   mode,
                   SH_DENYNONE,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#else
    handle = sopen(name, mode, SH_DENYNONE, S_IREAD | S_IWRITE);
#endif

    if(handle < 0)
    {
        fprintf(stderr, "Can't open file '%s'\n", name);
    }

    return handle;
}

void usage()
{
    printf("hptsqfix corrects errors in squish message bases\n");
    printf(
        "Usage: hptsqfix [options] areafilename ...\n" "Options:  -f\t- try to find next header after broken msg\n" "\t  -e\t- 'areafilename' has extension, strip it\n"
                                                                                                                    "\t  -u\t- undelete (restore deleted messages)\n");
    exit(-1);
}

/*
   static int cmp_msgEntry(const void *a, const void *b)
   {
    const sortedbase* r1 = (sortedbase*)a;
    const sortedbase* r2 = (sortedbase*)b;

    if( r1->msgTime > r2->msgTime)
        return 1;
    else if( r1->msgTime < r2->msgTime)
        return -1;
    return 0;
   }
 */
int findhdr(int SqdHandle)
{
    off_t pos;
    dword i = 0, rc = 1, stop = 0;
    unsigned char chr;
    unsigned char sqhdrid[] =
    {
        0x53, 0x44, 0xae, 0xaf
    };

    fprintf(stderr, "Searching valid header ID... ");
    pos = tell(SqdHandle);

    do
    {
        if(read(SqdHandle, &chr, 1) == 1)
        {
            pos++;

            if(chr == sqhdrid[i])
            {
                i++;

                if(i == 4)
                {
                    fprintf(stderr, " Wow! Found at pos %lu\n", (unsigned long)pos - 4);
                    lseek(SqdHandle, pos - 4, SEEK_SET);
                    rc   = 0;
                    stop = 1;
                }
            }
            else
            {
                if(chr == sqhdrid[0])
                {
                    i = 1;
                }
                else
                {
                    i = 0;
                }
            }
        }
        else
        {
            fprintf(stderr, " not found\n");
            stop = 1;
            rc   = 1;
        }
    }
    while(!stop);
    return rc;
} /* findhdr */

int Checkhdr(int SqdHandle, SQHDR * psqhdr)
{
    int stop = 0;

    if(read_sqhdr(SqdHandle, psqhdr) == 0)
    {
        fprintf(stderr, "... end");
        stop++;
    }

    if(stop == 0 && psqhdr->frame_length != psqhdr->msg_length)
    {
        fprintf(stderr,
                "\nFrame Length != Msg Length %u:%u\n",
                psqhdr->frame_length,
                psqhdr->msg_length);
    }

    if(stop == 0 && psqhdr->id != SQHDRID)
    {
        fprintf(stderr, "\nLooks like Message header is damaged\n");

        if(tryfind)
        {
            stop = findhdr(SqdHandle);

            if(stop == 0)
            {
                stop = -1;
            }
        }
        else
        {
            fprintf(stderr, "\nYou may want to use -f parameter\n");
            stop++;
        }
    }

    if(stop == 0 && psqhdr->msg_length <= XMSG_SIZE)
    {
        fprintf(stderr, "\nThe message body is too short: %u\n", psqhdr->msg_length);

        if(tryfind)
        {
            stop = findhdr(SqdHandle);

            if(stop == 0)
            {
                stop = -1;
            }
        }
        else
        {
            fprintf(stderr, "\nYou may want to use -f parameter\n");
            stop++;
        }
    }

    if(stop == 0 && psqhdr->frame_length > maxMsgLen)
    {
        fprintf(stderr,
                "\nThe frame is larger than the rest of the file\n"
                "Looks like the message header is damaged\n");

        if(tryfind)
        {
            stop = findhdr(SqdHandle);

            if(stop == 0)
            {
                stop = -1;
            }
        }
        else
        {
            fprintf(stderr, "\nYou may want to use -f parameter\n");
            stop++;
        }
    }

    return stop;
} /* Checkhdr */

#define SUCCESS 0
#define FAIL    1

int repair(char * areaName)
{
    int SqdHandle;
    int NewSqdHandle, NewSqiHandle;
    dword i;
    long saved = 0;
    int stop;
    int firstmsg = 1;
    int sayFree = 0;
    dword frame_length = 0;
    char * sqd, * newsqd, * newsqi, * text;
    SQBASE sqbase;
    SQHDR sqhdr;
    XMSG xmsg;
    SQIDX sqidx;

    sqd    = (char *)malloc(strlen(areaName) + 5);
    newsqd = (char *)malloc(strlen(areaName) + 5);
    newsqi = (char *)malloc(strlen(areaName) + 5);
    sprintf(sqd, "%s%s", areaName, EXT_SQDFILE);
    SqdHandle = Open_File(sqd, fop_rpb);

    if(SqdHandle == -1)
    {
        fprintf(stderr, "\nCannot open the file %s\n", sqd);
        nfree(sqd);
        nfree(newsqd);
        nfree(newsqi);
        return FAIL;
    }

    sprintf(newsqd, "%s.tmd", areaName);
    sprintf(newsqi, "%s.tmi", areaName);
    NewSqdHandle = Open_File(newsqd, fop_wpb);

    if(NewSqdHandle == -1)
    {
        fprintf(stderr, "\nCannot create the file %s\n", newsqd);
        nfree(sqd);
        nfree(newsqd);
        nfree(newsqi);
        close(SqdHandle);
        return FAIL;
    }

    NewSqiHandle = Open_File(newsqi, fop_wpb);

    if(NewSqiHandle == -1)
    {
        fprintf(stderr, "\nCannot create the file %s\n", newsqi);
        nfree(sqd);
        nfree(newsqd);
        nfree(newsqi);
        close(NewSqdHandle);
        close(SqdHandle);
        return FAIL;
    }

    lseek(SqdHandle, 0L, SEEK_END);
    maxMsgLen = tell(SqdHandle) - SQBASE_SIZE - SQHDR_SIZE;
    lseek(SqdHandle, 0L, SEEK_SET);

    if(lock(SqdHandle, 0, 1) != 0)
    {
        close(NewSqdHandle);
        close(NewSqiHandle);
        close(SqdHandle);
        nfree(sqd);
        nfree(newsqd);
        nfree(newsqi);
        fprintf(stderr, "\nCan't lock the message base\n");
        return 0;
    }

    read_sqbase(SqdHandle, &sqbase);
    sqbase.num_msg     = 0;
    sqbase.high_msg    = 0;
    sqbase.skip_msg    = 0;
    sqbase.begin_frame = SQBASE_SIZE;
    sqbase.last_frame  = sqbase.begin_frame;
    sqbase.free_frame  = sqbase.last_free_frame = 0;
    sqbase.end_frame   = sqbase.begin_frame;

    for(stop = 0, i = 1; !stop; i++)
    {
        fprintf(stderr, "Msg %u", i);
        stop = Checkhdr(SqdHandle, &sqhdr);

        if(stop == 1)
        {
            break;
        }

        if(stop == -1)
        {
            stop = 0;
            continue;
        }

        if(sqhdr.frame_type != FRAME_normal)
        {
            if(unDelete != 0 && sqhdr.frame_type == FRAME_free)
            {
                sqhdr.frame_type = FRAME_normal;
                fprintf(stderr, "... free, restoring\n");
            }
            else
            {
                sayFree = 1;
                fprintf(stderr, "... free\r");
                lseek(SqdHandle, sqhdr.frame_length, SEEK_CUR);
                continue;
            }
        }

        read_xmsg(SqdHandle, &xmsg);
        xmsg.attr    = 0;
        xmsg.replyto = 0;
        memset(xmsg.replies, '\000', sizeof(UMSGID) * MAX_REPLY);
        text = (char *)calloc(sqhdr.frame_length, sizeof(char *));
        farread(SqdHandle, text, (sqhdr.frame_length - XMSG_SIZE));
        /* memcpy(text, text, sqhdr.msg_length); */
        frame_length       = sqhdr.frame_length;
        sqhdr.frame_length = sqhdr.msg_length;

        if(firstmsg)
        {
            sqhdr.prev_frame = 0;
            firstmsg         = 0;
        }
        else
        {
            sqhdr.prev_frame = sqbase.last_frame;
        }

        sqhdr.next_frame  = tell(NewSqdHandle) + SQHDR_SIZE + sqhdr.msg_length;
        sqbase.last_frame = tell(NewSqdHandle);
        sqidx.ofs         = sqbase.last_frame;
        write_sqhdr(NewSqdHandle, &sqhdr);
        write_xmsg(NewSqdHandle, &xmsg);
        farwrite(NewSqdHandle, text, (sqhdr.msg_length - XMSG_SIZE));
        sqbase.end_frame = tell(NewSqdHandle);
        sqidx.hash       = SquishHash(xmsg.to);

        if(xmsg.attr & MSGREAD)
        {
            sqidx.hash |= (dword)0x80000000L;
        }

        sqidx.umsgid = sqbase.num_msg + 1;
        write_sqidx(NewSqiHandle, &sqidx, 1);
        nfree(text);
        sqbase.num_msg++;
        sqbase.high_msg = sqbase.num_msg;
        saved++;

        if(sayFree)
        {
            fprintf(stderr, "        ");
            sayFree = 0;
        }

        fprintf(stderr, "\r");
        maxMsgLen -= frame_length;
    }
    fprintf(stderr, "\n%u messages read\n", i - 2);
    lseek(NewSqiHandle, SQIDX_SIZE, SEEK_END);
    read_sqidx(NewSqiHandle, &sqidx, 1);
    lseek(NewSqdHandle, sqidx.ofs, SEEK_SET);
    read_sqhdr(NewSqdHandle, &sqhdr);
    sqhdr.next_frame = 0;
    lseek(NewSqdHandle, sqidx.ofs, SEEK_SET);
    write_sqhdr(NewSqdHandle, &sqhdr);
    lseek(NewSqdHandle, 0L, SEEK_SET);
    write_sqbase(NewSqdHandle, &sqbase);
    unlock(SqdHandle, 0, 1);
    close(NewSqdHandle);
    close(NewSqiHandle);
    close(SqdHandle);
    nfree(sqd);
    nfree(newsqd);
    nfree(newsqi);
    fprintf(stderr, "%ld messages saved\n", saved);
    return SUCCESS;
} /* repair */

int main(int argc, char * argv[])
{
    int i, j, extIndx, result = 0;
    int stripExt = 0;

    versionStr = GenVersionStr("hptsqfix", hptsqfix_VER_MAJOR, hptsqfix_VER_MINOR,
                               hptsqfix_VER_PATCH, hptsqfix_VER_BRANCH, cvs_date);
    printf("%s\n\n", versionStr);

    if(argc < 2)
    {
        usage();
    }

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(stricmp(argv[i], "-f") == 0)
            {
                tryfind = 1;
            }
            else if(stricmp(argv[i], "-e") == 0)
            {
                stripExt = 1;
            }
            else if(stricmp(argv[i], "-u") == 0)
            {
                unDelete = 1;
            }
            else
            {
                usage();
            }
        }
    }

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            if(stripExt)
            {
                extIndx = 0;

                for(j = 0; argv[i][j]; j++)
                {
                    if(argv[i][j] == '.')
                    {
                        extIndx = j;
                    }
                }

                if(extIndx > 0)
                {
                    argv[i][extIndx] = '\0';
                }
                else
                {
                    fprintf(stderr, "Warning: loose extension in '%s'\n", argv[i]);
                }
            }

            fprintf(stderr, "Repairing area '%s'\n", argv[i]);
            result = repair(argv[i]);
            if(result == SUCCESS)
            {
                fprintf(stderr, "Done\n\n");
            }
        }
    }
    return result;
} /* main */
