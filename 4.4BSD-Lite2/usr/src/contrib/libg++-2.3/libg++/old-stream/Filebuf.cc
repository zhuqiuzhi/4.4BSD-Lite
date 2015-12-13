/* 
Copyright (C) 1990 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of GNU CC.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.  No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.  Refer to the GNU CC General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute
GNU CC, but only under the conditions described in the
GNU CC General Public License.   A copy of this license is
supposed to have been given to you along with GNU CC so you
can know your rights and responsibilities.  It should be in a
file named COPYING.  Among other things, the copyright notice
and this notice must be preserved on all copies.  
*/

#if 1
#ifdef __GNUG__
#pragma implementation
#endif
#endif

#include <std.h>
#include <sys/file.h>           // needed to determine values of O_RDONLY...
#include <filebuf.h>

filebuf::filebuf() 
     :streambuf(), fd(-1), opened(0) {}

filebuf::filebuf(int newfd) 
     : streambuf(), fd(newfd), opened(1) {}

filebuf::filebuf(int newfd, char* buf, int buflen)
     : streambuf(buf, buflen), fd(newfd), opened(1) {}

int filebuf::is_open()
{
  return opened;
}

int filebuf::close()
{
  int was = opened;
  if (was) ::close(fd);
  opened = 0;
  return was;
}

streambuf* filebuf::open(const char* name, open_mode m)
{
  if (opened) return 0;
  int mode = -1; // any illegal value
  switch (m)
  {
  case input: mode = O_RDONLY; 
              break;
  case output: mode = O_WRONLY | O_CREAT | O_TRUNC;
              break;
  case append: mode = O_APPEND | O_CREAT | O_WRONLY;
              break;
  }
  fd = ::open(name, mode, 0666);
  if (opened = (fd >= 0))
  {
    allocate();
    return this;
  }
  else
    return 0;
}


streambuf*  filebuf::open(const char* filename, io_mode m, access_mode a)
{
  return 0;
}

streambuf* filebuf::open(const char* filename, const char* m)
{
  return 0;
}

streambuf*  filebuf::open(int  filedesc, io_mode m)
{
  return 0;
}

streambuf*  filebuf::open(FILE* fileptr)
{
  return 0;
}

int filebuf::underflow()
{
  if (!opened) return EOF;
  if (base == 0) allocate();
  int nwanted = eptr - base + 1;
  int nread = ::read(fd, base, nwanted);
  if (nread >= 0)
  {
    gptr = base;
    pptr = base + nread;
  }
  return (nread <= 0)? EOF : int(*gptr);
}

int filebuf::overflow(int ch)
{
  if (!opened) return EOF;
  if (base == 0) allocate();
  if (ch != EOF)             // overflow *must* be called before really full
    *pptr++ = (char)(ch);

  // loop, in case write can't handle full request
  // From: Rene' Seindal <seindal@diku.dk>

  int w, n, t;
  for (w = t = 0, n = pptr - base; n > 0; n -= w, t += w) 
  {
    if ((w = ::write(fd, base + t, n)) < 0)
      break;
  }
 
  pptr = base;
  return (n == 0 && w >= 0)? 0 : EOF;
}

filebuf::~filebuf()
{
  close();
}
 Fp = new File(filename, m, a);
  init_streambuf_ptrs();
}

Filebuf::Filebuf(const char* filename, const char* m)
     : streambuf()
{
  Fp = new File(filename, m);
  init_streambuf_ptrs();
}

Filebuf::Filebuf(int filedesc, io_mode m)
     : streambuf()
{
  Fp = new File(filedesc, m);
  init_streambuf_ptrs();
}

Filebuf::Filebuf(FILE* fileptr)
     : streambuf()
{
  Fp = new File(fileptr);
  init_streambuf_ptrs();
}

int Filebuf::close()
{
  int was = Fp->is_open();
  if (was) { overflow(); Fp->close(); }
#ifdef VMS
  if (was) {
	if(alloc && (base != 0)) {delete base; base=0; alloc = 0;};
	gptr=0;};
#endif
  return was;
}


Filebuf::~Filebuf()
{
  if (Fp != 0)
  {
    close();
    delete Fp;
  }
}

#ifdef VMS
/*
	VMS implementation notes:
 The underflow routine works fine as long as there is no mixing of input and
 output for the same stream.  If this were to happen, then great confusion
 would occur, since we maintain a seperate buffer for the case of output.
 the stream classes do not allow this as they are currently written, so for
 now we are OK.  VMS does not handle buffered output in quite the same way
 that UNIX does, so a seperate buffer is allocated, and used by the program.
 when it comes time to flush it we call write(...) to empty it.  The flush
 function under VMS does not flush the buffer unless it is full, and whatsmore
 the _iobuf is not 14 bytes long as one might suspect from the structure def,
 but at *least* 66 bytes long.  Some of these hidden structure elements need
 to be set for the output to work properly, and it seemed to be poor
 programming practice to fool with them
*/
#endif
/*
  The underflow and overflow methods sync the streambuf with the _iobuf
  ptrs on the way in and out of the read. I believe that this is
  done in a portable way.
*/
int Filebuf::underflow()
{
  int ch;
  if (Fp == 0) return EOF;
  if (gptr == 0) // stdio _iobuf ptrs not initialized until after 1st read
  {
#ifdef VMS
	assert(alloc==0);
#endif
    ch = Fp->fill();
    base = FPOINT->_base;
    eptr = base - 1 + _bufsiz(Fp->fp);
  }
  else
  {
    FPOINT->_ptr = gptr;
    FPOINT->_cnt = eptr - gptr + 1;
    ch = Fp->fill();
  }
  gptr = base;
  *gptr = ch;
  if (ch == EOF)
    pptr = base;
  else
    pptr = base + FPOINT->_cnt + 1;
  if (Fp->good())
    return ch;
  else
  {
    Fp->clear();
    return EOF;
  }
}

int Filebuf::overflow(int ch)
{
  if (Fp == 0) return EOF;
  if (FPOINT->_flag & _IONBF)  // handle unbuffered IO specially
  {
    if (pptr == 0) // 1st write
    {
      if (ch == EOF)
        return 0;
      else
      {
        Fp->flush(ch);
      }
    }
    else
    {
      if (ch == EOF)
        Fp->flush();		// Probably not necessary
      else
        Fp->flush(ch);
    }
  }
  else
  {
    if (pptr == 0) // 1st write
    {
      if (ch == EOF)
        return 0;
      else
      {
        Fp->flush(ch);
#ifdef VMS
        base = new char[BUFSIZ];
        alloc = 1;
#else
        base = FPOINT->_base;
#endif
        eptr = base - 1 + _bufsiz(Fp->fp);
        gptr = base;
      }
    }
    else
    {
      if (ch != EOF) *pptr++ = ch;
#ifdef VMS
      if(gptr==base) write(FPOINT->_file,base,pptr-base);
#else
      FPOINT->_ptr = pptr;
      FPOINT->_cnt = eptr - pptr + 1;
#endif
      Fp->flush();
    }
#ifdef VMS
    pptr = base;
#else
    pptr = FPOINT->_ptr;
#endif
  }
  if (Fp->fail() || Fp->bad())
  {
    Fp->clear(); // this allows recovery from ostream level
    return EOF;
  }
  else
    return 0;
}

const char* Filebuf::name()
{
  return Fp->name();
}

streambuf* Filebuf::setbuf(char* buf, int buflen, int preload)
{
  if (preload != 0) return 0; // cannot preload, sorry
  if (Fp != 0) Fp = new File;
  Fp->setbuf(buflen, buf);
  init_streambuf_ptrs();
  return (Fp->good())? this : 0;
}

void Filebuf::error()
{
  Fp->error();
}
