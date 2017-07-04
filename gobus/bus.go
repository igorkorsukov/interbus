package gotoc

import "fmt"

type Method func(args []interface{}) (interface{}, error)

type Bus interface {
	BindMethod(method string, fn Method)
	CallMethod(method string, res interface{}, args ...interface{}) (interface{}, error)
}

type cbus struct {
	transport Transport
	methods   map[string]Method

	debug bool
}

var _g_bus *cbus

func InitBusDefault() Bus {
	t := NewTransportDefault(Port_Bus)
	return InitBus(t)
}

func InitBus(t Transport) Bus {

	_g_bus = &cbus{transport: t}

	t.Listen(_g_bus)

	return _g_bus
}

func GetBus() Bus {
	return _g_bus
}

func (b *cbus) BindMethod(method string, fn Method) {

	if b.methods == nil {
		b.methods = make(map[string]Method)
	}
	b.methods[method] = fn
}

func (b *cbus) CallMethod(method string, res interface{}, args ...interface{}) (interface{}, error) {

	if b.transport == nil {
		panic(fmt.Sprint("gotoc: channel not inited"))
	}
	return b.transport.Call(method, args, res)
}

func (b *cbus) OnCalled(method string, args []interface{}) (interface{}, error) {

	if b.debug {
		fmt.Println("gotoc: CalledHandler method: ", method, ", args: ", args)
	}

	m, ok := b.methods[method]
	if !ok {
		return nil, fmt.Errorf("gotoc: not found method: %s", method)
	}
	return m(args)
}
