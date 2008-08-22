#ifndef X_FRAME_BUFFER_H_
#define X_FRAME_BUFFER_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>  // XmbSetWMProperties()

#include "common/verify.h"

class XFrameBuffer
{
private:
    static const uint32_t m_bpp = 4;

    uint32_t m_width;
    uint32_t m_height;

    Window m_window;
    bool m_isWindowReady;

    XImage* m_pXimage;
    GC m_gc;
    Display* m_pDisplay;
    uint8_t* m_pFrameBuffer;

public:
    XFrameBuffer(uint32_t width, uint32_t height)
        : m_width(width),
          m_height(height),
          m_isWindowReady(false),
          m_pXimage(0),
          m_pDisplay(0),
          m_pFrameBuffer(0)
    {
    }

    ~XFrameBuffer()
    {
        fin();
    }

    bool init()
    {
        if (XInitThreads() == 0)
            VERIFY(0);

        if ((m_pDisplay = XOpenDisplay(0)) == 0)
            VERIFY(0);

        m_window = XCreateSimpleWindow(m_pDisplay, DefaultRootWindow(m_pDisplay),
                                       0, 0, m_width, m_height, 3, WhitePixel(m_pDisplay, 0), BlackPixel(m_pDisplay, 0));

        if(m_window == BadAlloc || m_window == BadMatch || m_window == BadValue || m_window == BadWindow)
        {
            fin();
            VERIFY(0);
        }
        m_isWindowReady = true;

        XmbSetWMProperties(m_pDisplay, m_window, "Simple window", 0, 0, 0, 0, 0, 0);

        // XSelectInput(s_dpy, s_window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
        XMapWindow(m_pDisplay, m_window);
        XFlush(m_pDisplay);

        if (!(m_pFrameBuffer = (uint8_t*)malloc(m_width * m_height * m_bpp)))
        {
            fin();
            VERIFY(0);
        }

        if (!(m_pXimage = XCreateImage(m_pDisplay, CopyFromParent, DefaultDepth(m_pDisplay, DefaultScreen(m_pDisplay)),
                                       ZPixmap, 0, (char*)m_pFrameBuffer, m_width, m_height, m_bpp * 8, m_bpp * m_width)))
        {
            fin();
            VERIFY(0);
        }
    
        XGCValues xgcv;
        m_gc = XCreateGC(m_pDisplay, m_window, 0, &xgcv);
        XFlush(m_pDisplay);
        XSync(m_pDisplay, False);
    }

    void fin()
    {
        if (m_pXimage)
        {
            XFree(m_pXimage);
            m_pXimage = 0;
        }

        free(m_pFrameBuffer);
        m_pFrameBuffer = 0;

        if (m_pDisplay)
        {
            if (m_isWindowReady)
                XDestroyWindow(m_pDisplay, m_window);

            XCloseDisplay(m_pDisplay);
            m_pDisplay = 0;
            m_isWindowReady = false;
        }
    }

    void flip()
    {
        XPutImage(m_pDisplay, m_window, m_gc, m_pXimage, 0, 0, 0, 0, m_width, m_height);
        XFlush(m_pDisplay);
        XSync(m_pDisplay, False);
    }

    uint8_t* getBuffer(uint32_t x = 0, uint32_t y = 0) { return m_pFrameBuffer + (y * m_width + x) * m_bpp; }
    uint32_t getWidth() { return m_width; }
    uint32_t getHeight() { return m_height; }
    uint32_t getBpp() { return m_bpp; }
};

#endif // X_FRAME_BUFFER_H_
