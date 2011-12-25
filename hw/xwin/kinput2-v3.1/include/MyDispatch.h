/* $Id: MyDispatch.h,v 1.3 1991/02/06 10:54:31 ishisone Rel $ */

/*
 *	a simple event dispatch library for non-widget windows
 */

extern void MyDispatchEvent(
#if NeedFunctionPrototypes
	XEvent *	/* event */
#endif
);

extern void MyAddEventHandler(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	Window		/* window */,
	int		/* type */,
	unsigned long	/* mask */,
	void (*)()	/* func */,
	XtPointer	/* data */
#endif
);

extern void MyRemoveEventHandler(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	Window		/* window */,
	int		/* type */,
	void (*)()	/* func */,
	XtPointer	/* data */
#endif
);

extern void MyRemoveAllEventHandler(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	Window		/* window */
#endif
);


#ifdef USE_WINIME
extern int winXIMEXEventHandler (
#if NeedFunctionPrototypes
	Display * pDisplay,
	XEvent * event,
	int ime_event_base,
	int ime_error_base
#endif
);
#endif
