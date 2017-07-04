package gotoc

import (
	"fmt"
)

type Logger interface {
	Error(msg interface{}, keyvals ...interface{})
	Warn(msg interface{}, keyvals ...interface{})
	Info(msg interface{}, keyvals ...interface{})
	Debug(msg interface{}, keyvals ...interface{})
}

type clog struct {
	transport Socket
}

func NewLogger() Logger {
	return clog{transport: GetSocket()}
}

func (l clog) Print(tp string, a ...interface{}) {
	str := fmt.Sprint(tp, ": ", a)
	fmt.Println("gotoc: ", str)
	l.transport.Send(Port_Log, []byte(str), nil)
}

func (l clog) Error(msg interface{}, keyvals ...interface{}) {
	l.Print("ERROR", msg, keyvals)
}

func (l clog) Warn(msg interface{}, keyvals ...interface{}) {
	l.Print("WARN", msg, keyvals)
}

func (l clog) Info(msg interface{}, keyvals ...interface{}) {
	l.Print("INFO", msg, keyvals)
}

func (l clog) Debug(msg interface{}, keyvals ...interface{}) {
	l.Print("DEBUG", msg, keyvals)
}
