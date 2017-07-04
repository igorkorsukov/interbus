package main

import "C"

import (
	"fmt"
	"unsafe"

	"github.com/igorkorsukov/interbus/gobus"
)

//export GoToC_Init
func GoToC_Init(c_call unsafe.Pointer) {

	fmt.Println("go: GoToC_Init c_call 42: ", c_call)
}

//export GoToC_Call
func GoToC_Call(fi int, in_p unsafe.Pointer, in_size C.int) (unsafe.Pointer, int) {
	//return gotoc.Go_Call(fi, in_p, int(in_size))
}

func main() {}
