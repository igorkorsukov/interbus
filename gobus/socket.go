package gotoc

/*
#include <stdlib.h>
static inline void call_c_call(void* fp, int i, void* in_p, int in_size, void** out_p, int* out_size) {
 	void (*Fn)(int, void*, int, void**, int*);
	Fn = (void(*)(int, void*, int, void**, int*))(fp);
    (*Fn)(i, in_p, in_size, out_p, out_size);
}
*/
import "C"
import (
	"fmt"
	"unsafe"
)

type OnReply interface {
	OnReply(ret []byte)
}

type Handler interface {
	OnRequest(in []byte, out *[]byte)
}

type Socket interface {
	Send(port int, data []byte, h OnReply)
	Listen(port int, h Handler)
}

var _g_csocket *csocket

type csocket struct {
	c_call unsafe.Pointer
	list   map[int]Handler
	debug  bool
}

func InitSocket(c_call unsafe.Pointer) Socket {

	_g_csocket = &csocket{c_call: c_call}

	_g_csocket.Listen(Port_Ping, &ping{})

	return _g_csocket
}

func GetSocket() Socket {
	return _g_csocket
}

func (t *csocket) Send(port int, in []byte, onrep OnReply) {

	if t.c_call == nil {
		panic(fmt.Sprint("gotoc: socket not inited"))
	}

	p := C.CBytes(in)

	if t.debug {
		fmt.Println("gotoc: socket.Send port: ", port, ", data: ", string(in), ", reply: ", onrep, ", c_call: ", t.c_call)
	}

	if onrep == nil {

		C.call_c_call(t.c_call, C.int(port), p, C.int(len(in)), nil, nil)

	} else {

		var out_p unsafe.Pointer
		var out_size C.int
		C.call_c_call(t.c_call, C.int(port), p, C.int(len(in)), &out_p, &out_size)

		if out_p != nil && out_size > 0 {
			out_b := C.GoBytes(out_p, out_size)
			C.free(out_p)
			onrep.OnReply(out_b)
		}
	}
}

func (t *csocket) Listen(port int, h Handler) {

	if t.list == nil {
		t.list = make(map[int]Handler)
	}

	if h != nil {
		_, ok := t.list[port]
		if ok {
			fmt.Println("gotoc: alredy listen port: ", port)
			return
		}
		t.list[port] = h
	} else {
		delete(t.list, port)
	}
}

func Go_OnRequest(fi int, in_p unsafe.Pointer, in_size int) (unsafe.Pointer, int) {
	if _g_csocket != nil {
		return _g_csocket.onRequest(fi, in_p, in_size)
	}
	return nil, 0
}

func (t *csocket) onRequest(port int, in_p unsafe.Pointer, in_size int) (unsafe.Pointer, int) {

	h, ok := t.list[port]
	if ok {

		var in_b []byte
		if in_p != nil && in_size > 0 {
			in_b = C.GoBytes(in_p, C.int(in_size))
			C.free(in_p)
		}

		var out_b []byte
		h.OnRequest(in_b, &out_b)

		if t.debug {
			fmt.Println("gotoc: request port: ", port, ", in size: ", len(in_b), ", out_size: ", len(out_b))
		}

		if len(out_b) > 0 {
			p := C.CBytes(out_b)
			return p, len(out_b)
		}

	} else {
		if t.debug {
			fmt.Println("gotoc: request not found port: ", port)
		}
	}
	return nil, 0
}
