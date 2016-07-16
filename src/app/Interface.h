/*
Music Explorer
Copyright (c) 2003 Geoff Peters and Gabriel Lo

This file is part of Music Explorer.

Music Explorer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Music Explorer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Music Explorer (COPYING.TXT); if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
+===========================================================================+
|				Copyright (C) Direct Line Corp. 1999-2000.					|
+---------------------------------------------------------------------------+
| File Name:																|
|																			|
|	Interface.h																|
|																			|
+---------------------------------------------------------------------------+
| Descriptions:																|
|																			|
|	Private interfaces definition and implementaion.						|
|																			|
+---------------------------------------------------------------------------+
| Developer(s):																|
|																			|
|	Xu Wen Bin.																|
|																			|
+===========================================================================+
|                         C H A N G E     L O G                             |
+---------------------------------------------------------------------------+
|																			|
|	07-20-01	1.00	Created.											|
|	09-01-01	1.10	Modified.											|
|																			|
+---------------------------------------------------------------------------+
| Notes:																	|
|																			|
+===========================================================================+
*/

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <afxtempl.h>

/****************************************************************************
 * IIUnknown interface definition and implementation.
 ****************************************************************************
 * We use IIUnknown instead of IUnknown to prevent name space conflict.
 */
class IIUnknown
{
	virtual long AddRef() = 0;
	virtual long Release() = 0;
};

#define IMP_IIUNKNOWN() \
	long AddRef();      \
	long Release()

/****************************************************************************
 * IIStreamDevice interface definition and implementation.
 ****************************************************************************
 * Standard stream device interface.
 */
class IIStreamDevice
{
	virtual BOOL Open(UINT uDirection)         = 0;
	virtual BOOL Close(UINT uDirection)        = 0;
	virtual BOOL Start(UINT uDirection)        = 0;
	virtual BOOL Stop(UINT uDirection)         = 0;
	virtual BOOL Pause(UINT uDirection)        = 0;
	virtual BOOL Restart(UINT uDirection)      = 0;
	virtual BOOL Reset(UINT uDirection)        = 0;
	virtual UINT GetDirection()                = 0;
	virtual void SetDirection(UINT uDirection) = 0;
};

#define IMP_IISTREAMDEVICE()       \
	BOOL Open(UINT uDirection);    \
	BOOL Close(UINT uDirection);   \
	BOOL Start(UINT uDirection);   \
	BOOL Stop(UINT uDirection);    \
	BOOL Pause(UINT uDirection);   \
	BOOL Restart(UINT uDirection); \
	BOOL Reset(UINT uDirection);   \
	UINT GetDirection();           \
	void SetDirection(UINT uDirection)

/****************************************************************************
 * IIDataSource interface definition and implementation.
 ****************************************************************************
 * Standard data source interface.
 */
template<class DATATYPE>
class IIDataSource
{
	virtual BOOL Read(DATATYPE * ppData)  = 0;
	virtual BOOL ReadDone(DATATYPE pData) = 0;
};

#define IMP_IIDATASOURCE(DATATYPE) \
	BOOL Read(DATATYPE * ppData);  \
	BOOL ReadDone(DATATYPE pData)

/****************************************************************************
 * IIDataSink interface definition and implementation.
 ****************************************************************************
 * Standard data sink interface.
 */
template<class DATATYPE>
class IIDataSink
{
	virtual BOOL Write(DATATYPE * ppData) = 0;
	virtual BOOL Write(DATATYPE pData)    = 0;
};

#define IMP_IIDATASINK(DATATYPE)   \
	BOOL Write(DATATYPE * ppData); \
	BOOL Write(DATATYPE pData)

/****************************************************************************
 * IIAdvise interface definition and implementation.
 ****************************************************************************
 * Standard advise interface.
 */
#define CALLBACK_TYPEMASK   0x00070000l     /* callback type mask */
#define CALLBACK_NULL       0x00000000l     /* no callback */
#define CALLBACK_WINDOW     0x00010000l     /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l     /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l     /* dwCallback is a FARPROC */
#define CALLBACK_THREAD     (CALLBACK_TASK) /* thread ID replaces 16 bit task */
#define CALLBACK_EVENT      0x00050000l     /* dwCallback is an EVENT Handle */

typedef void (* PEVENTSINKFUNC) (DWORD, DWORD, DWORD, DWORD);

typedef struct stAdviseStructure
{
	DWORD	dwCallBack; // Represent HWDN, THREAD_ID, TAKS_ID or CALLBACK FUNCTION.
	DWORD	dwInstance;
	DWORD   dwMask;
	DWORD   dwType;
} ADVISESTRUCTURE, *PADVISESTRUCTURE;

class IIAdvise
{
	virtual void Advise(DWORD dwCallBack, DWORD dwInstance, DWORD dwMask, DWORD dwType) = 0;
	virtual void Unadvise(DWORD dwCallBack) = 0;
	virtual void Invoke(DWORD dwEvent, DWORD wParam, DWORD lParam) = 0;
};

#define IMP_IIADVISE()                                                           \
	void Advise(DWORD dwCallBack, DWORD dwInstance, DWORD dwMask, DWORD dwType); \
	void Unadvise(DWORD dwCallBack);                                             \
	void Invoke(DWORD dwEvent, DWORD wParam, DWORD lParam)

class CAdvise : public IIAdvise
{
public:

	CAdvise()
	{
		InitializeCriticalSection(&m_adviseListLock);
	}

	~CAdvise()
	{
		DeleteCriticalSection(&m_adviseListLock);

		while (!m_adviseList.IsEmpty())
		{
			PADVISESTRUCTURE stAdvise = m_adviseList.RemoveHead();

			if (stAdvise)
			{
				delete stAdvise;
			}
		}
	}

	void Advise(DWORD dwCallBack, DWORD dwInstance, DWORD dwMask, DWORD dwType)
	{
		PADVISESTRUCTURE stAdvise = new ADVISESTRUCTURE;
		stAdvise->dwCallBack = dwCallBack;
		stAdvise->dwInstance = dwInstance;
		stAdvise->dwMask     = dwMask;
		stAdvise->dwType     = dwType;

		EnterCriticalSection(&m_adviseListLock);

		m_adviseList.AddTail(stAdvise);

		LeaveCriticalSection(&m_adviseListLock);
	}

	void Unadvise(DWORD dwCallBack)
	{
		EnterCriticalSection(&m_adviseListLock);

		int count = m_adviseList.GetCount();

		for (int i = 0; i < count; i++)
		{
			PADVISESTRUCTURE stAdvise = m_adviseList.RemoveHead();

			if (stAdvise->dwCallBack == dwCallBack)
			{
				delete stAdvise;
				stAdvise = NULL;
				break;
			}
			else
			{
				m_adviseList.AddTail(stAdvise);
			}
		}

		LeaveCriticalSection(&m_adviseListLock);
	}

	void Invoke(DWORD dwEvent, DWORD wParam, DWORD lParam)
	{
		EnterCriticalSection(&m_adviseListLock);

		if (!m_adviseList.IsEmpty())
		{
			PADVISESTRUCTURE stAdvise = NULL;

			int count = m_adviseList.GetCount();

			for (int i = 0; i < count; i++)
			{
				stAdvise = m_adviseList.RemoveHead();

				if (stAdvise)
				{
					if (stAdvise->dwType == CALLBACK_EVENT)
					{
						::SetEvent((HANDLE)stAdvise->dwCallBack);
					}
					else if (stAdvise->dwType == CALLBACK_FUNCTION)
					{
						if (dwEvent & stAdvise->dwMask)
						{
							(PEVENTSINKFUNC(stAdvise->dwCallBack))(dwEvent, stAdvise->dwInstance, wParam, lParam);
						}
					}
					else if (stAdvise->dwType == CALLBACK_WINDOW)
					{
						if (dwEvent & stAdvise->dwMask)
						{
							::SendMessage((HWND)stAdvise->dwCallBack, dwEvent, wParam, lParam);
						}
					}
					else if (stAdvise->dwType == CALLBACK_THREAD)
					{
						if (dwEvent & stAdvise->dwMask)
						{
							::PostThreadMessage(stAdvise->dwCallBack, dwEvent, wParam, lParam);
						}
					}
					else
					{
						// CALLBACK_TASK.
					}

					// Add advise element back to list.
					m_adviseList.AddTail(stAdvise);
				}
			}
		}

		LeaveCriticalSection(&m_adviseListLock);
	}

private:

	// Advise list.
	CRITICAL_SECTION m_adviseListLock;
	CList<PADVISESTRUCTURE,PADVISESTRUCTURE&> m_adviseList;
};

/****************************************************************************
 * IDLEventSink interface  definition and implementation.
 ****************************************************************************
 * Standard event sink interface.
 */
class IIEventSink
{
	virtual void OnEvent(DWORD dwEvent, DWORD dwInstance, DWORD wParam, DWORD lParam) = 0;
};

#define IMP_IIEVENTSINK() \
	void OnEvent(DWORD dwEvent, DWORD dwInstance, DWORD wParam, DWORD lParam)

#endif // _INTERFACE_H_
